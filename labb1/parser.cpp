//
// Created by Bashar Levin on 2024-01-31.
//

#include "parser.h"

/*
<match> := <exp>
<exp> := <group> | <or> | <many> | <counter>
<group> := (exp)
<word> := <string> | <char>
<char> := a-z | A-Z
<string> := <word>(<char> <word>) | <digits>
<digits> := 0-9 <digits>
<or> := <exp> + <exp>
<many> := <exp>*
<counter> := <exp>{<digits>}
 */