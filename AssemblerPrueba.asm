MOV EAX,DS
MOV b[EAX+10],100 
MOV EDX,EAX
ADD EDX,10
MOV CL,1
MOV CH,1
MOV AL,1
SYS 2
STOP