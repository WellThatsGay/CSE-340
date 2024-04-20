#include "execute.h"
#include "lexer.h"
#include <vector>
#include <algorithm>
#include "parser.h"


/*Christopher Harris*/

// Create an instance of LexicalAnalyzer named lexer
LexicalAnalyzer lexer;

using namespace std;

// Declare a vector of strings named table
vector<string> table;

// Forward declaration of parse_body function
struct InstructionNode * parse_body();

// Function to find the location of a string in the table
int location(string str) {
    for (int i = 0; i < table.size(); i++) {
        if (table[i] == str)
            return i;
    }
}

// Function to parse identifiers and add them to the table
void parse_id_list() {
    Token t = lexer.GetToken();

    // Loop until a SEMICOLON token is found
    while (t.token_type != SEMICOLON) {
        table.push_back(t.lexeme);
        mem[location(t.lexeme)] = 0;
        t = lexer.GetToken();
    }
}

// Function to parse assignment statements
struct InstructionNode * parse_assign_stmt() {
    // Create a new InstructionNode and initialize its next pointer to NULL
    struct InstructionNode * instruction = new InstructionNode;
    instruction->next = NULL;

    // Get the first token from the lexer
    Token t = lexer.GetToken();

    // If the token is an ID, set the type of the instruction to ASSIGN and set the left_hand_side_index
    if (t.token_type == ID) {
        instruction->type = ASSIGN;
        instruction->assign_inst.left_hand_side_index = location(t.lexeme);
    }

    // Get the next token
    t = lexer.GetToken();

    // If the token is EQUAL, process the right hand side of the assignment
    if (t.token_type == EQUAL) {
        t = lexer.GetToken();

        // Depending on the type of the token, set the operand1_index
        switch (t.token_type)
        {
        case ID:
            instruction->assign_inst.opernd1_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }

            instruction->assign_inst.opernd1_index = location(t.lexeme);
            break;
        }

        // Get the next token
        t = lexer.GetToken();

        // If the token is a SEMICOLON, the assignment is complete
        if (t.token_type == SEMICOLON) {
            instruction->assign_inst.op = OPERATOR_NONE;
            instruction->assign_inst.opernd2_index = NULL;
        }
        else {
            // If the token is an operator, set the op field accordingly
            if (t.token_type == PLUS)
                instruction->assign_inst.op = OPERATOR_PLUS;
            else if (t.token_type == MINUS)
                instruction->assign_inst.op = OPERATOR_MINUS;
            else if (t.token_type == MULT)
                instruction->assign_inst.op = OPERATOR_MULT;
            else if (t.token_type == DIV)
                instruction->assign_inst.op = OPERATOR_DIV;

            // Get the next token and set the operand2_index
            t = lexer.GetToken();
            switch (t.token_type)
            {
            case ID:
                instruction->assign_inst.opernd2_index = location(t.lexeme);
                break;
            case NUM:
                // If the lexeme is not in the table, add it and initialize its memory location
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }

                instruction->assign_inst.opernd2_index = location(t.lexeme);
                break;
            }
            // Get the next token (should be a SEMICOLON)
            t = lexer.GetToken();
        }
    }

    // Return the constructed instruction
    return instruction;
}

// Function to parse while statements
struct InstructionNode * parse_while_stmt() {
    // Create a new InstructionNode and initialize its next pointer to NULL
    struct InstructionNode * instruction = new InstructionNode;
    instruction->next = NULL;

    // Get the first token from the lexer
    Token t = lexer.GetToken();

    // If the token is WHILE, process the while statement
    if (t.token_type == WHILE) {
        // Set the type of the instruction to CJMP (conditional jump)
        instruction->type = CJMP;

        // Process the first operand
        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instruction->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instruction->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        }

        // Process the condition operator
        t = lexer.GetToken();
        if (t.token_type == GREATER)
            instruction->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            instruction->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        // Process the second operand
        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instruction->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instruction->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        }

        // Parse the body of the while loop
        instruction->next = parse_body();

        // Create a jump node that jumps back to the start of the loop
        struct InstructionNode* jumpNode = new InstructionNode;
        jumpNode->type = JMP;
        jumpNode->jmp_inst.target = instruction;

        // Append the jump node to the end of the body
        struct InstructionNode* temp = instruction->next;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = jumpNode;

        // Create a noop node to serve as the target of the conditional jump
        struct InstructionNode * noop = new InstructionNode;
        noop->type = NOOP;
        noop->next = NULL;
        jumpNode->next = noop;

        // Set the target of the conditional jump to the noop node
        instruction->cjmp_inst.target = noop;
    }

    // Return the constructed instruction
    return instruction;
}

// Function to parse if statements
struct InstructionNode * parse_if_stmt() {
    // Create a new InstructionNode and initialize its next pointer to NULL
    struct InstructionNode * instruction = new InstructionNode;
    instruction->next = NULL;

    // Get the first token from the lexer
    Token t = lexer.GetToken();

    // If the token is IF, process the if statement
    if (t.token_type == IF)
    {
        // Set the type of the instruction to CJMP (conditional jump)
        instruction->type = CJMP;

        // Process the first operand
        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instruction->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instruction->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        }

        // Process the condition operator
        t = lexer.GetToken();
        if (t.token_type == GREATER)
            instruction->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            instruction->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        // Process the second operand
        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instruction->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instruction->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        }

        // Parse the body of the if statement
        instruction->next = parse_body();

        // Create a noop node to serve as the target of the conditional jump
        struct InstructionNode * noop = new InstructionNode;
        noop->type = NOOP;
        noop->next = NULL;

        // Append the noop node to the end of the body
        struct InstructionNode * temp = instruction->next;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = noop;

        // Set the target of the conditional jump to the noop node
        instruction->cjmp_inst.target = noop;
    }

    // Return the constructed instruction
    return instruction;
}

// Function to parse a case or default statement
struct InstructionNode * parse_case(InstructionNode * jump, int temp_operand) {
    // Create a new InstructionNode for the case or default statement
    struct InstructionNode * instruction = new InstructionNode;

    // Set the type of the node and the index of the first operand
    instruction->type = CJMP;
    instruction->cjmp_inst.opernd1_index = temp_operand;
    instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

    // Get the first token from the lexer
    Token t = lexer.GetToken(); // case or default
    switch (t.token_type) {
        case CASE:
            t = lexer.GetToken(); // Get the next token
            // Set the index of the second operand based on whether the token is an ID or a NUM
            // ...

        case DEFAULT:
            // Set the index of the second operand to the index of the first operand
            instruction->cjmp_inst.opernd2_index = temp_operand;
            break;
    }

    t = lexer.GetToken(); // colon

    // Parse the body of the case or default statement and set it as the target of the conditional jump
    instruction->cjmp_inst.target = parse_body();

    // Append the jump node to the end of the body
    struct InstructionNode * temp = instruction->cjmp_inst.target;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = jump;

    // If the next token is a case or default statement, parse it and append it to the current statement
    t = lexer.peek(1);
    if (t.token_type == DEFAULT || t.token_type == CASE) {
        struct InstructionNode * i = parse_case(jump, temp_operand);
        instruction->next = i;
    }

    // Return the parsed case or default statement
    return instruction;
}

// Function to parse a switch statement
struct InstructionNode * parse_switch_stmt() {
    // Create new InstructionNodes for the switch statement structure
    struct InstructionNode * instruction;
    struct InstructionNode * jump = new InstructionNode;
    struct InstructionNode * noop = new InstructionNode;

    // Set the types of the nodes
    noop->type = NOOP;
    noop->next = NULL;
    jump->type = JMP;
    jump->jmp_inst.target = noop;

    int temp_operand;

    // Parse the switch statement
    Token t = lexer.GetToken(); // switch
    // Set temp_operand based on whether the next token is an ID or a NUM
    // ...

    t = lexer.GetToken(); // {

    // Parse the first case or default statement
    instruction = parse_case(jump, temp_operand);

    // Append the noop node to the end of the switch statement
    struct InstructionNode * temp = instruction;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = noop;

    t = lexer.GetToken(); // }

    // Return the parsed switch statement
    return instruction;
}

struct InstructionNode * parse_for_stmt() {
    // Create new InstructionNodes for the for loop structure
    struct InstructionNode * instruction = new InstructionNode; // Conditional jump node
    struct InstructionNode * noop = new InstructionNode; // No operation node
    struct InstructionNode * jump = new InstructionNode; // Jump node

    // Set the types of the nodes
    instruction->type = CJMP;
    noop->type = NOOP;
    jump->type = JMP;

    // Set the targets for the jump instructions
    instruction->cjmp_inst.target = noop;
    jump->jmp_inst.target = instruction;

    // Set the next nodes
    jump->next = noop;
    noop->next = NULL;

    // Parse the for loop structure
    Token t = lexer.GetToken(); // for
    t = lexer.GetToken();  // (
    struct InstructionNode * i1 = parse_assign_stmt(); // Initial assignment
    i1->next = instruction; 

    // Parse the condition
    t = lexer.GetToken(); // id or num
    // Set operand1_index based on whether the token is an ID or a NUM

    t = lexer.GetToken(); // condition
    // Set condition_op based on the token type

    t = lexer.GetToken(); // id or num
    // Set operand2_index based on whether the token is an ID or a NUM

    // Parse the increment/decrement operation and the body of the for loop
    t = lexer.GetToken(); //  ;
    struct InstructionNode * i2 = parse_assign_stmt(); // Increment/decrement operation
    t = lexer.GetToken();  // )
    struct InstructionNode * i_list = parse_body(); // Body of the for loop
    instruction->next = i_list;

    // Append the increment/decrement operation to the end of the body
    struct InstructionNode * temp = i_list;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = i2;

    // Set the next node of the increment/decrement operation to the jump node
    i2->next = jump;

    // Return the initial assignment node
    return i1;
}

struct InstructionNode * parse_output_stmt() {
	struct InstructionNode * instruction = new InstructionNode;
	instruction->next = NULL;

	Token t = lexer.GetToken(); // OUTPUT
	instruction->type = OUT;

	t = lexer.GetToken(); // ID
	instruction->input_inst.var_index = location(t.lexeme);

	t = lexer.GetToken(); // SEMICOLON

	return instruction;
} 

struct InstructionNode * parse_input_stmt() {
	struct InstructionNode * instruction = new InstructionNode;
	instruction->next = NULL;

	Token t = lexer.GetToken(); // INPUT
	instruction->type = IN;

	t = lexer.GetToken(); // ID
	instruction->input_inst.var_index = location(t.lexeme);

	t = lexer.GetToken(); // SEMICOLON

	return instruction;
} 

// Function to parse a single statement
struct InstructionNode * parse_stmt() {
    // Create a new InstructionNode
    struct InstructionNode * i1;

    // Get the first token from the lexer
    Token t = lexer.peek(1);

    // Depending on the type of the token, parse the corresponding statement
    switch (t.token_type) 
    {
        case ID:
            i1 = parse_assign_stmt();
            break;
        case WHILE:
            i1 = parse_while_stmt();
            break;
        case IF:
            i1 = parse_if_stmt();
            break;
        case SWITCH:
            i1 = parse_switch_stmt();
            break;
        case FOR:
            i1 = parse_for_stmt();
            break;
        case OUTPUT:
            i1 = parse_output_stmt();
            break;
        case INPUT:
            i1 = parse_input_stmt();
            break;
    }

    // Return the parsed statement
    return i1;
}

// Function to parse a list of statements
struct InstructionNode * parse_stmt_list() {
    // Create two InstructionNodes
    struct InstructionNode * i1;
    struct InstructionNode * i2;

    // Get the first token from the lexer
    Token t = lexer.peek(1);
    TokenType t_type = t.token_type;

    // Parse the first statement
    i1 = parse_stmt();

    // Get the next token
    t = lexer.peek(1);

    // If the token is the start of a statement, parse the rest of the statement list
    if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF ||
        t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT) {
        i2 = parse_stmt_list();

        // If the first statement is a conditional jump or a for loop, append the rest of the statement list to the end of the body
        if (i1->type == CJMP || t_type == FOR) {
            struct InstructionNode * temp = i1->next;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = i2;
        }
        else {
            // Otherwise, just append the rest of the statement list to the first statement
            i1->next = i2;
        }
    }

    // Return the parsed statement list
    return i1;
}

// Function to parse a body of code enclosed in braces
struct InstructionNode * parse_body() {
    // Create an InstructionNode
    struct InstructionNode * instruction;

    // Get the first token from the lexer
    Token t = lexer.GetToken();

    // If the token is a left brace, parse the statement list inside the braces
    if (t.token_type == LBRACE)
    {
        instruction = parse_stmt_list();
    }

    // Get the right brace token
    t = lexer.GetToken();
    
    // Return the parsed body
    return instruction;
}

// Function to parse inputs
void parse_inputs() {
    // Get the first token from the lexer
    Token t = lexer.GetToken();

    // Convert the token's lexeme to an integer and add it to the inputs
    inputs.push_back(stoi(t.lexeme));

    // Get the next token
    t = lexer.peek(1); 

    // If the token is a number, parse the rest of the inputs
    if (t.token_type == NUM)
        parse_inputs();
}

// Function to generate an intermediate representation of the code
struct InstructionNode * parse_Generate_Intermediate_Representation() {

    parse_id_list(); 
    // Parse the body of the code
    struct InstructionNode * temp = parse_body();
    parse_inputs();

    // Return the intermediate representation
    return temp;
}