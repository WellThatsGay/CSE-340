#include "execute.h"
#include "lexer.h"
#include <vector>
#include <algorithm>

/* Christopher Harris*/

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
            instruction->assign_inst.operand1_index = location(t.lexeme);
            break;
        case NUM:
            // If the lexeme is not in the table, add it and initialize its memory location
            if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                table.push_back(t.lexeme);
                mem[location(t.lexeme)] = stoi(t.lexeme);
            }

            instruction->assign_inst.operand1_index = location(t.lexeme);
            break;
        }

        // Get the next token
        t = lexer.GetToken();

        // If the token is a SEMICOLON, the assignment is complete
        if (t.token_type == SEMICOLON) {
            instruction->assign_inst.op = OPERATOR_NONE;
            instruction->assign_inst.operand2_index = NULL;
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
                instruction->assign_inst.operand2_index = location(t.lexeme);
                break;
            case NUM:
                // If the lexeme is not in the table, add it and initialize its memory location
                if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
                    table.push_back(t.lexeme);
                    mem[location(t.lexeme)] = stoi(t.lexeme);
                }

                instruction->assign_inst.operand2_index = location(t.lexeme);
                break;
            }
            // Get the next token (should be a SEMICOLON)
            t = lexer.GetToken();
        }
    }

    // Return the constructed instruction
    return instruction;
}

struct InstructionNode * parse_while_stmt() {
	struct InstructionNode * instruction = new InstructionNode;
	instruction->next = NULL;

	Token t = lexer.GetToken();
	if (t.token_type == WHILE) {

		instruction->type = CJMP;
		//operand 1
		t = lexer.GetToken();
		switch (t.token_type)
		{
		case ID:
			instruction->cjmp_inst.operand1_index = location(t.lexeme);
			break;
		case NUM:
			if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
				table.push_back(t.lexeme);
				mem[location(t.lexeme)] = stoi(t.lexeme);
			}
			instruction->cjmp_inst.operand1_index = location(t.lexeme);
			break;
		}
		//condition op
		t = lexer.GetToken();
		if (t.token_type == GREATER)
			instruction->cjmp_inst.condition_op = CONDITION_GREATER;
		else if (t.token_type == LESS)
			instruction->cjmp_inst.condition_op = CONDITION_LESS;
		else if (t.token_type == NOTEQUAL)
			instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

		//operand 2
		t = lexer.GetToken();
		switch (t.token_type)
		{
		case ID:
			instruction->cjmp_inst.operand2_index = location(t.lexeme);
			break;
		case NUM:
			if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
				table.push_back(t.lexeme);
				mem[location(t.lexeme)] = stoi(t.lexeme);
			}

			instruction->cjmp_inst.operand2_index = location(t.lexeme);
			break;
		}

		instruction->next = parse_body();

		struct InstructionNode* jumpNode = new InstructionNode;
		jumpNode->type = JMP;
		jumpNode->jmp_inst.target = instruction;

		struct InstructionNode* temp = instruction->next;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = jumpNode;

		struct InstructionNode * noop = new InstructionNode;
		noop->type = NOOP;
		noop->next = NULL;
		jumpNode->next = noop;

		instruction->cjmp_inst.target = noop;
	}
	return instruction;
}

struct InstructionNode * parse_if_stmt() {
	struct InstructionNode * instruction = new InstructionNode;
	instruction->next = NULL;

	Token t = lexer.GetToken();
	if (t.token_type == IF)
	{
		instruction->type = CJMP;
		//operand 1
		t = lexer.GetToken();
		switch (t.token_type)
		{
		case ID:
			instruction->cjmp_inst.operand1_index = location(t.lexeme);
			break;
		case NUM:
			if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
				table.push_back(t.lexeme);
				mem[location(t.lexeme)] = stoi(t.lexeme);
			}
			instruction->cjmp_inst.operand1_index = location(t.lexeme);
			break;
		}
		//condition op
		t = lexer.GetToken();
		if (t.token_type == GREATER)
			instruction->cjmp_inst.condition_op = CONDITION_GREATER;
		else if (t.token_type == LESS)
			instruction->cjmp_inst.condition_op = CONDITION_LESS;
		else if (t.token_type == NOTEQUAL)
			instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

		//operand 2
		t = lexer.GetToken();
		switch (t.token_type)
		{
		case ID:
			instruction->cjmp_inst.operand2_index = location(t.lexeme);
			break;
		case NUM:
			if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
				table.push_back(t.lexeme);
				mem[location(t.lexeme)] = stoi(t.lexeme);
			}

			instruction->cjmp_inst.operand2_index = location(t.lexeme);
			break;
		}

		instruction->next = parse_body();

		struct InstructionNode * noop = new InstructionNode;
		noop->type = NOOP;
		noop->next = NULL;
		
		struct InstructionNode * temp = instruction->next;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = noop;
		instruction->cjmp_inst.target = noop;
	}
	return instruction;
}

struct InstructionNode * parse_case(InstructionNode * jump, int temp_operand) {
	struct InstructionNode * instruction = new InstructionNode;

	instruction->type = CJMP;
	instruction->cjmp_inst.operand1_index = temp_operand;
	instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

	Token t = lexer.GetToken(); // case or defualt
	switch (t.token_type) {
		case CASE:
			t = lexer.GetToken();
			if (t.token_type == ID) {
				instruction->cjmp_inst.operand2_index = location(t.lexeme);
			}
			else if (t.token_type == NUM) {
				if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
					table.push_back(t.lexeme);
					mem[location(t.lexeme)] = stoi(t.lexeme);
				}
				instruction->cjmp_inst.operand2_index = location(t.lexeme);
				break;
			}		
		case DEFAULT:
			instruction->cjmp_inst.operand2_index = temp_operand;
			break;
	}

	t = lexer.GetToken(); // colon

	instruction->cjmp_inst.target = parse_body();

	struct InstructionNode * temp = instruction->cjmp_inst.target;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = jump;

	t = lexer.GetToken();
	lexer.UngetToken(t);
	if (t.token_type == DEFAULT || t.token_type == CASE) {
		
		struct InstructionNode * i = parse_case(jump, temp_operand);
		instruction->next = i;
	}

	return instruction;
}

struct InstructionNode * parse_switch_stmt() {
	struct InstructionNode * insturction;
	struct InstructionNode * jump = new InstructionNode;
	struct InstructionNode * noop = new InstructionNode;

	noop->type = NOOP;
	noop->next = NULL;
	jump->type = JMP;
	jump->jmp_inst.target = noop;

	int temp_opreand;

	Token t = lexer.GetToken();
	if (t.token_type == SWITCH) {
		
		t = lexer.GetToken();
		if(t.token_type == ID) 
			temp_opreand = location(t.lexeme);
		else if (t.token_type == NUM) {
			if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
				table.push_back(t.lexeme);
				mem[location(t.lexeme)] = stoi(t.lexeme);
			}

			temp_opreand = location(t.lexeme);
		}

		t = lexer.GetToken();// {

		insturction = parse_case(jump, temp_opreand);

		struct InstructionNode * temp = insturction;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = noop;
	
		t = lexer.GetToken(); //}

		return insturction;
	}
}

struct InstructionNode * parse_for_stmt() {
	struct InstructionNode * instruction = new InstructionNode;
	struct InstructionNode * noop = new InstructionNode;
	struct InstructionNode * jump = new InstructionNode;
	
	instruction->type = CJMP;
	noop->type = NOOP;
	jump->type = JMP;
	instruction->cjmp_inst.target = noop;
	jump->jmp_inst.target = instruction;
	jump->next = noop;
	noop->next = NULL;

	Token t = lexer.GetToken(); // for
	t = lexer.GetToken();  // (
	struct InstructionNode * i1 = parse_assign_stmt(); // asign 1
	i1->next = instruction; 

	t = lexer.GetToken(); // id or num
	if (t.token_type == ID)
		instruction->cjmp_inst.operand1_index = location(t.lexeme);
	else if (t.token_type == NUM) {
		if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
			table.push_back(t.lexeme);
			mem[location(t.lexeme)] = stoi(t.lexeme);
		}
		instruction->cjmp_inst.operand1_index = location(t.lexeme);
	}

	t = lexer.GetToken(); // condtion
	switch (t.token_type) {
		case GREATER:
			instruction->cjmp_inst.condition_op = CONDITION_GREATER;
		case LESS:
			instruction->cjmp_inst.condition_op = CONDITION_LESS;
		case NOTEQUAL:
			instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
	}

	t = lexer.GetToken(); // id or num
	if (t.token_type == ID)
		instruction->cjmp_inst.operand2_index = location(t.lexeme);
	else if (t.token_type == NUM) {
		if (find(table.begin(), table.end(), t.lexeme) == table.end()) {
			table.push_back(t.lexeme);
			mem[location(t.lexeme)] = stoi(t.lexeme);
		}
		instruction->cjmp_inst.operand2_index = location(t.lexeme);
	}
	
	t = lexer.GetToken(); //  ;
	struct InstructionNode * i2 = parse_assign_stmt(); // assign 2
	t = lexer.GetToken();  // )
	struct InstructionNode * i_list = parse_body();
	instruction->next = i_list;

	struct InstructionNode * temp = i_list;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = i2;
	
	i2->next = jump;
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

struct InstructionNode * parse_stmt() {
	struct InstructionNode * i1;
	Token t = lexer.GetToken();
	switch (t.token_type) 
	{
		case ID:
			lexer.UngetToken(t);
			i1 = parse_assign_stmt();
			break;
		case WHILE:
			lexer.UngetToken(t);
			i1 = parse_while_stmt();
			break;
		case IF:
			lexer.UngetToken(t);
			i1 = parse_if_stmt();
			break;
		case SWITCH:
			lexer.UngetToken(t);
			i1 = parse_switch_stmt();
			break;
		case FOR:
			lexer.UngetToken(t);
			i1 = parse_for_stmt();
			break;
		case OUTPUT:
			lexer.UngetToken(t);
			i1 = parse_output_stmt();
			break;
		case INPUT:
			lexer.UngetToken(t);
			i1 = parse_input_stmt();
			break;
	}
	return i1;
}

struct InstructionNode * parse_stmt_list() {
	struct InstructionNode * i1;
	struct InstructionNode * i2;
	Token t = lexer.GetToken();
	TokenType t_type = t.token_type;
	lexer.UngetToken(t);

	i1 = parse_stmt();

	t = lexer.GetToken();
	if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF ||
		t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT) {

		lexer.UngetToken(t);
		i2 = parse_stmt_list();

		if (i1->type == CJMP || t_type == FOR) {
			struct InstructionNode * temp = i1->next;
			while (temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = i2;
		}
		else {
			i1->next = i2;
		}
	}
	else {
		lexer.UngetToken(t);
	}
	return i1;
}

struct InstructionNode * parse_body() {
	struct InstructionNode * instruction;
	Token t = lexer.GetToken();
	//get left brace
	if (t.token_type == LBRACE)
	{
		instruction = parse_stmt_list();
	}
	//get right brace
	t = lexer.GetToken();
	
	return instruction;
}

void parse_inputs() {
	Token t = lexer.GetToken();
	inputs.push_back(stoi(t.lexeme));
	t = lexer.GetToken(); lexer.UngetToken(t);
	if (t.token_type == NUM)
		parse_inputs();
}

struct InstructionNode * parse_generate_intermediate_representation() {
	// first line is the delcartion of variables
	parse_id_list(); 
	//parse body
	struct InstructionNode * temp = parse_body();
	//parse last line which is input
	parse_inputs();
	return temp;
}

