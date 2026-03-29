%option noyywrap c++
%option never-interactive
%x PROTOCOL SERVER ZONE NAME_ZONE SLASH PAGE FILE TAIL

%%
<INITIAL>{
    ^https?            BEGIN(PROTOCOL);
    [ \t]+             ;           
    \n                 return 0;         
    .                  return 0;        
}
<PROTOCOL>{
    :\/\/              BEGIN(SERVER);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<SERVER>{
    [a-z]+             BEGIN(ZONE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<ZONE>{
    \.                 BEGIN(NAME_ZONE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<NAME_ZONE>{
    [a-zA-Z]+          BEGIN(SLASH);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<SLASH>{
    \/                 BEGIN(PAGE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<PAGE>{
    [a-zA-Z0-9]+       BEGIN(FILE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<FILE>{
    \.html             { BEGIN(TAIL); return 1; } 
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<TAIL>{
    .                  ;               
    \n                 BEGIN(INITIAL); 
}
<<EOF>> {
    return 0;         
}
%%
