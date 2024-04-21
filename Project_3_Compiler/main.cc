#include "execute.h"
#include "lexer.h"
#include <vector>
#include <algorithm>
#include "parser.h"

LexicalAnalyzer lexer;
using namespace std;

/*Christopher Harris was trying to fix the Switch Stmt*/

vector<string> table;

// Declaration of parsing functions
struct InstructionNode * parse_body();

int location(string str) {
    for (int i = 0; i < table.size(); i++) {
        if (table[i] == str)
            return i;
    }
}


void parse_id_list() {
    Token token = lexer.GetToken();
    while (token.token_type != SEMICOLON) {
        table.push_back(token.lexeme);  
        mem[location(token.lexeme)] = 0;      
        token = lexer.GetToken();             
    }
}

struct InstructionNode * parse_assign_stmt() {
    struct InstructionNode * instParseAssignStmt = new InstructionNode;
    instParseAssignStmt->next = NULL;
    Token t = lexer.GetToken();

    if (t.token_type == ID) {
        instParseAssignStmt->type = ASSIGN;
        instParseAssignStmt->assign_inst.left_hand_side_index = location(t.lexeme);
    }

    t = lexer.GetToken();  
    if (t.token_type == EQUAL) {
        t = lexer.GetToken();  
        switch (t.token_type) {
            case ID:
                instParseAssignStmt->assign_inst.opernd1_index = location(t.lexeme);
                break;
            case NUM:
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                instParseAssignStmt->assign_inst.opernd1_index = location(t.lexeme);
                break;
             case END_OF_FILE:
            // Handle the END_OF_FILE case appropriately
            // This might include logging an error, cleaning up resources, or breaking the loop
            delete instParseAssignStmt;  // Clean up before returning
            return nullptr;  // Return a null pointer to indicate an error

        default:
            // Optionally handle any other unanticipated token types
            break;
        }

        t = lexer.GetToken();  
        if (t.token_type == SEMICOLON) {
            instParseAssignStmt->assign_inst.op = OPERATOR_NONE;
            instParseAssignStmt->assign_inst.opernd2_index = NULL;
        } else {
            // Determine the operator and get the second operand
            if (t.token_type == PLUS)
                instParseAssignStmt->assign_inst.op = OPERATOR_PLUS;
            else if (t.token_type == MINUS)
                instParseAssignStmt->assign_inst.op = OPERATOR_MINUS;
            else if (t.token_type == MULT)
                instParseAssignStmt->assign_inst.op = OPERATOR_MULT;
            else if (t.token_type == DIV)
                instParseAssignStmt->assign_inst.op = OPERATOR_DIV;

            t = lexer.GetToken();  // Get the next token to identify the second operand
            switch (t.token_type) {
                case ID:
                    instParseAssignStmt->assign_inst.opernd2_index = location(t.lexeme);
                    break;
                case NUM:
                    // Add numeric value to the table if it's not already present
                    if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                        table.push_back(t.lexeme);
                        mem[location(t.lexeme)] = stoi(t.lexeme);
                    }
                    instParseAssignStmt->assign_inst.opernd2_index = location(t.lexeme);
                    break;
            }
            t = lexer.GetToken();  
        }
    }

    return instParseAssignStmt;  // Return the constructed instruction node for the assignment statement
}

struct InstructionNode * parse_while_stmt() {
    struct InstructionNode * instWhileStmt = new InstructionNode;
    instWhileStmt->next = NULL;

    Token t = lexer.GetToken();  // Expecting a WHILE token

    if (t.token_type == WHILE) {
        instWhileStmt->type = CJMP;

        t = lexer.GetToken();  // This should be the condition variable or value
        switch (t.token_type) {
            case ID:
                instWhileStmt->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
            case NUM:
                // If numeric value is not already present, add it to the table and initialize in memory
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                instWhileStmt->cjmp_inst.opernd1_index = location(t.lexeme);
                break;
        }

        t = lexer.GetToken();  // This should be the comparison operator
        if (t.token_type == GREATER)
            instWhileStmt->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            instWhileStmt->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            instWhileStmt->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        t = lexer.GetToken();  // This should be the operand for comparison
        switch (t.token_type) {
            case ID:
                instWhileStmt->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
            case NUM:
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                instWhileStmt->cjmp_inst.opernd2_index = location(t.lexeme);
                break;
        }

        instWhileStmt->next = parse_body();  // Parse the body of the while loop

        // Create and link a jump node to implement the loop back to the start of the while
        struct InstructionNode* jumpNode = new InstructionNode;
        jumpNode->type = JMP;
        jumpNode->jmp_inst.target = instWhileStmt;

        struct InstructionNode* temp = instWhileStmt->next;
        while (temp->next != NULL) {
            temp = temp->next;  
        }
        temp->next = jumpNode;  

        struct InstructionNode * noop = new InstructionNode;  
        noop->type = NOOP;
        noop->next = NULL;
        jumpNode->next = noop;  // Link jump node to noop

        instWhileStmt->cjmp_inst.target = noop;  
    }

    return instWhileStmt;  // Return the constructed instruction node for the while loop
}

struct InstructionNode * parse_if_stmt() {
    struct InstructionNode * instIfStmt = new InstructionNode;
    instIfStmt->next = NULL;

    Token t = lexer.GetToken();

    if (t.token_type == IF)
    {
        instIfStmt->type = CJMP;

        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instIfStmt->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        case NUM:
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instIfStmt->cjmp_inst.opernd1_index = location(t.lexeme);
            break;
        }

        t = lexer.GetToken();
        if (t.token_type == GREATER)
            instIfStmt->cjmp_inst.condition_op = CONDITION_GREATER;
        else if (t.token_type == LESS)
            instIfStmt->cjmp_inst.condition_op = CONDITION_LESS;
        else if (t.token_type == NOTEQUAL)
            instIfStmt->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        t = lexer.GetToken();
        switch (t.token_type)
        {
        case ID:
            instIfStmt->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        case NUM:
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            instIfStmt->cjmp_inst.opernd2_index = location(t.lexeme);
            break;
        }

        instIfStmt->next = parse_body();

        struct InstructionNode * noop = new InstructionNode;
        noop->type = NOOP;
        noop->next = NULL;

        struct InstructionNode * temp = instIfStmt->next;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = noop;

        instIfStmt->cjmp_inst.target = noop;
    }

    return instIfStmt;
}

struct InstructionNode * parse_case(InstructionNode * jump, int temp_operand) {
    struct InstructionNode * instParseCase = new InstructionNode;
    instParseCase->type = CJMP;
    instParseCase->cjmp_inst.opernd1_index = temp_operand;
    instParseCase->cjmp_inst.condition_op = CONDITION_NOTEQUAL;  

    Token t = lexer.GetToken();  
    switch (t.token_type) {
        case CASE:
            t = lexer.GetToken();  
            if (t.token_type == ID) {
                
                instParseCase->cjmp_inst.opernd2_index = location(t.lexeme);
            }
            else if (t.token_type == NUM) {
                // Add numeric value to the table if it's not already present
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }
                instParseCase->cjmp_inst.opernd2_index = location(t.lexeme);
            }
            break;
        case DEFAULT:
            
            //instParseCase->cjmp_inst.opernd2_index = temp_operand;
            break;
    }

    t = lexer.GetToken();  

    // Parse the body of the case statement
    instParseCase->cjmp_inst.target = parse_body();

    // Link the jump node to the end of the case statement
    struct InstructionNode * temp = instParseCase->cjmp_inst.target;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = jump;  

    // Check if there are more case statements to parse
    t = lexer.peek(1);
    if (t.token_type == DEFAULT || t.token_type == CASE) {
        struct InstructionNode * i = parse_case(jump, temp_operand);
        instParseCase->next = i;  
    }

    return instParseCase;  // Return the constructed instruction node for the case statement
}

struct InstructionNode * parse_switch_stmt() {
    struct InstructionNode * instSwitchStmt = nullptr;  // Initialized to nullptr
    struct InstructionNode * jump = new InstructionNode;
    struct InstructionNode * noop = new InstructionNode;

    noop->type = NOOP;
    noop->next = NULL;
    jump->type = JMP;
    jump->jmp_inst.target = noop;

    int temp_operand = -1;  // Initialize with a default invalid index

    Token t = lexer.GetToken();
    if (t.token_type == SWITCH) {
        t = lexer.GetToken();
        if (t.token_type == ID) {
            temp_operand = location(t.lexeme);
        } else if (t.token_type == NUM) {
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }
            temp_operand = location(t.lexeme);
        } else {
            // Handle syntax error or unexpected token type
        }

        t = lexer.GetToken();

        instSwitchStmt = parse_case(jump, temp_operand);

        // Ensure instSwitchStmt is not nullptr before linking the noop node
        if (instSwitchStmt != nullptr) {
            struct InstructionNode * temp = instSwitchStmt;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = noop;
        } else {
            // Handle the case where instSwitchStmt is nullptr, if necessary
        }

        t = lexer.GetToken();  // Potentially redundant and could be removed if not needed

        return instSwitchStmt;
    }
    return nullptr;  // Return nullptr if the SWITCH token is not found
}

struct InstructionNode * parse_for_stmt() {
    struct InstructionNode * instForStmt = new InstructionNode; 
    struct InstructionNode * noop = new InstructionNode;  
    struct InstructionNode * jump = new InstructionNode;  
    
    instForStmt->type = CJMP;
    noop->type = NOOP;
    jump->type = JMP;
    instForStmt->cjmp_inst.target = noop;  
    jump->jmp_inst.target = instForStmt;  
    jump->next = noop;  
    noop->next = NULL;

    Token t = lexer.GetToken();  
    t = lexer.GetToken();  

    struct InstructionNode * instOne = parse_assign_stmt();  // Parse the initialization statement.
    instOne->next = instForStmt;  

    t = lexer.GetToken();  
    if (t.token_type == ID)
        instForStmt->cjmp_inst.opernd1_index = location(t.lexeme);  
    else if (t.token_type == NUM) {
        if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
            table.push_back(t.lexeme);
            mem[location(t.lexeme)] = stoi(t.lexeme);
        }
        instForStmt->cjmp_inst.opernd1_index = location(t.lexeme); 
    }

    t = lexer.GetToken();  // Expecting the condition operator (>, <, !=).
    switch (t.token_type) {
        case GREATER:
            instForStmt->cjmp_inst.condition_op = CONDITION_GREATER;
            break;
        case LESS:
            instForStmt->cjmp_inst.condition_op = CONDITION_LESS;
            break;
        case NOTEQUAL:
            instForStmt->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
            break;
    }

    t = lexer.GetToken();   // Expecting the second operand for the condition.
    if (t.token_type == ID)
        instForStmt->cjmp_inst.opernd2_index = location(t.lexeme);
    else if (t.token_type == NUM) {
        if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
            table.push_back(t.lexeme);
            mem[location(t.lexeme)] = stoi(t.lexeme);
        }
        instForStmt->cjmp_inst.opernd2_index = location(t.lexeme);
    }
    
    t = lexer.GetToken();  

    struct InstructionNode * instTwo = parse_assign_stmt();  // Parse the update statement.
    t = lexer.GetToken();  

    struct InstructionNode * i_list = parse_body();  
    instForStmt->next = i_list;  

    struct InstructionNode * temp = i_list;
    while (temp->next != NULL) {
        temp = temp->next;  
    }
    temp->next = instTwo;  
    
    instTwo->next = jump;  
    return instOne;  
}


struct InstructionNode * parse_output_stmt() {
	struct InstructionNode * instOutputStmt = new InstructionNode;
	instOutputStmt->next = NULL;

	Token t = lexer.GetToken();
	instOutputStmt->type = OUT;

	t = lexer.GetToken();
	instOutputStmt->input_inst.var_index = location(t.lexeme);

	t = lexer.GetToken();

	return instOutputStmt;
} 

struct InstructionNode * parse_input_stmt() {
	struct InstructionNode * instInputStmt = new InstructionNode;
	instInputStmt->next = NULL;

	Token t = lexer.GetToken(); // INPUT
	instInputStmt->type = IN;

	t = lexer.GetToken(); // ID
	instInputStmt->input_inst.var_index = location(t.lexeme);

	t = lexer.GetToken(); // SEMICOLON

	return instInputStmt;
} 

struct InstructionNode * parse_stmt() {
    struct InstructionNode * instOne;

    Token t = lexer.peek(1);

    switch (t.token_type) 
    {
        case ID:
            instOne = parse_assign_stmt();
            break;
        case WHILE:
            instOne = parse_while_stmt();
            break;
        case IF:
            instOne = parse_if_stmt();
            break;
        case SWITCH:
            instOne = parse_switch_stmt();
            break;
        case FOR:
            instOne = parse_for_stmt();
            break;
        case OUTPUT:
            instOne = parse_output_stmt();
            break;
        case INPUT:
            instOne = parse_input_stmt();
            break;
    }

    return instOne;
}

struct InstructionNode * parse_stmt_list() {
    // These nodes represent individual statements or blocks of statements.
    struct InstructionNode * instOne;  // First statement in the list.
    struct InstructionNode * instTwo;  // Second statement in the list.

    // Peek the next token.
    Token t = lexer.peek(1);
    TokenType t_type = t.token_type;

    // Parse the first statement.
    instOne = parse_stmt();

    t = lexer.peek(1);

    // Check if the next token indicates another statement; continue parsing if true.
    if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF ||
        t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT) {
        instTwo = parse_stmt_list();  // Recursively parse the following statements.

        // Special handling for control flow structures that may need to link back to a previous statement.
        if (instOne->type == CJMP || t_type == FOR) {
            struct InstructionNode * temp = instOne->next;  
            while (temp->next != NULL) {
    
                temp = temp->next;
            }
            temp->next = instTwo;  
        } else {
           
            instOne->next = instTwo;
        }
    }

    // Return the first statement in the list.
    return instOne;
}


struct InstructionNode * parse_body() {
    struct InstructionNode * instructionParseBody;

    Token t = lexer.GetToken();

    if (t.token_type == LBRACE)
    {
        instructionParseBody = parse_stmt_list();
    }

    t = lexer.GetToken();
    
    return instructionParseBody;
}

void parse_inputs() {
    Token t = lexer.GetToken();

    inputs.push_back(stoi(t.lexeme));

    t = lexer.peek(1); 

    if (t.token_type == NUM)
        parse_inputs();
}

struct InstructionNode * parse_Generate_Intermediate_Representation() {
    
    parse_id_list(); 

    // Parse the main body of the program.
    struct InstructionNode * temp = parse_body();

    // Parse inputs if any.
    parse_inputs();

    return temp;
}
