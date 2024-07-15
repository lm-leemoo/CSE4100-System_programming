[system programming lecture]

-project 2 baseline

csapp.{c,h}
        CS:APP3e functions

shellex.c
        Simple shell example

명령어를 background로 돌아갈 때의 상황을 추가한 simple shell이다.
background로 돌아간 process를 처리하기 위해 signal handler가 필요하다.
jobs 명령어로 현재 돌아가거나 멈춰 있는 background process를 출력하기 위해서는
background process들을 저장할 배열이 필요하다.
저장한 배열에 새로운 process를 추가하고,
kill이나 종료로 인해  저장된 process 없애는 등의 작업을 처리해야 한다.

