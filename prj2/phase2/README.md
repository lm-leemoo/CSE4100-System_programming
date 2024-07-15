[system programming lecture]

-project 2 baseline

csapp.{c,h}
        CS:APP3e functions

shellex.c
        Simple shell example

단일 명령어 뿐만 아니라 여러개의 명령어가 파이프로 연결되어 있는 경우를
포함하여 명령어를 처리해주는 simple shell의 구조이다.
단일 명령어는 phase1의 코드를 사용하여 따로 if문으로 빼서 작동한다.
파이프로 두 개의 명령어를 연결하기 위해서는 pipe함수, 읽기 쓰기 파일 디스크립터가 필요하고,
STDIN과 STDOUT을 파이프로 연결시키기 위해 dup2()함수를 이용한다.
파일 디스크립터는 사용이 종료되면 close를 해줘야한다.

