# Grammar Specification

The following is a grammar specification using a custom notation:

```plaintext
<match> := <exp>
<exp> := <group> | <or> | <many> | <counter>
<group> := (exp)
<string> := <char>{n} 
<char> := a-z | A-Z | 0-9 | 
<digit> := 0-9  /
<word> := <char>{1,}    
<or> := <exp> | <exp> (not <exp>)  
<many> := <exp>*
<counter> := <exp>{n}
