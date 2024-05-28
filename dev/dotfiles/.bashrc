#!/bin/bash

 Cyan='\033[1;36m'
Green='\033[1;32m'
  Red='\033[1;31m'
 Blue='\033[1;34m'
Reset='\033[0m'

PS1="$Red\u$Reset @\h $Cyan\w$Reset [\j] % "

alias    ls='ls -lFa --color'
alias  path='echo $PATH | tr ":" "\n" | nl'
alias "::"="clear && "

