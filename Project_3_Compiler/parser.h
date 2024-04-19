#include <string>
#include "lexer.h"

/*Christopher Harris*/

class Parser {
private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
public:
    void parse_program();//
    void parse_var_sction(); //MIGTH NEED TO CHANGE
    void parse_id_list();//
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_expr();
    void parse_primary();
    void parse_op();
    void parse_output_stmt();
    void parse_input_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    void parse_condition();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();
    void parse_inputs();
    void parse_num_list();
};