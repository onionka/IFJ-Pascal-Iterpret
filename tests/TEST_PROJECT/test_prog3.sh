#!/usr/bin/env bash
export dir="prog3"
export name="prog3"
export file="../src/LEX_lexem.c"
export src='#include "../src/../src/inc.h"

int main () {
    return 0;
}'
export input="
{ Program 3: Prace s retezci a vestavenymi funkcemi }
var
  str1 : string;
  str2 : string;
  n : integer;
begin
  str1 := 'Toto je nejaky text';
  str2 := copy(str1, 9, 11);
  write(str1, ''#10'', str2, ''#10'');
  n := find(str2, 'text');
  write('Pozice retezce "text" v str2: ', n, ''#10'');
  write('Zadejte posloupnost vsech malych pismen a-h, ',
        'aby se pismena v posloupnosti neopakovala:');
  readln(str1);
  str2 := sort(str1);
  while str2 <> 'abcdefgh' do
  begin
    write('Spatne zadana posloupnost, zkuste znovu:'#10'');
    readln(str1);
    str2 := sort(str1)
  end
end.
"
export output=""
export retCode=0
