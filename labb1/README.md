# Grammar Specification

The following is a grammar specification using a custom notation:

```plaintext
<match> ::= <exp>
<exp> ::= <group> | <or> | <sub> | <word>
<sub> ::= <many> | <counter>
<group> ::= '(' <exp> ')'
<word> ::= <char> | <char> <word> | <sub>
<char> ::= 'a'-'z' | 'A'-'Z' | <digits> | <wildcard>
<digits> ::= '0'-'9' { <digits> }
<or> ::= <exp> '+' <exp>
<many> ::= <exp> '*'
<counter> ::= <exp> '{' <digits> '}'
<ignore> ::= <exp> '\I'
<output_group> ::= <gropu>\O{ditgits}
<wildcard> ::= '.'
