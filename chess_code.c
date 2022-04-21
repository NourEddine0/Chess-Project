#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <windows.h>
#include<math.h>
                    // ########### Declaration of Global Varaiables ###########
int player1_x=0;                    // 1) Removed pieces index
int player1_y=12;
int player2_x=0;
int player2_y=1;
int playerTurn=1;                   // 2) Variable to renter the input if it was invalid (Prevent turn changes)
int king1_x=8;                      // 3) King position indexes
int king1_y=7;
int king2_x=1;
int king2_y=7;
int checkUponKing=0;                    // 4) Check detector
int castlingValid[6] = {0};         // 5) indicator if any castling piece has been moved
int enPassant= -1;                  // 6) En Passant checking varaibles
int enPassantFlag = 0;
int enPassantPlayer = 0;
int turn =0;                        // 7) number of turns played
int uCounter =-1;                   // 8) File save & load variables
int fileCurrentSize;
int winDrawSwitch=0;                // switch between win and draw in draw function
int ignore=0;
char f1[14] = "ChessData.txt";
char f2[15] = "ChessData1.txt";
void readInput(char inp[],int Size){        // Function to read input
    int ch,i=0;
   while((ch=getchar())!='\n'){
    if (i<Size){
        inp[i++]=ch;
    }
   }
}

int checkKnightValidMove (int move_Piece[],int size1,int turn,char chessBoard[][14]){
    int g=0;
    if(turn%2==0){                   // Check if player 1 or 2
    g=32;
    }else{
    g=0;
    }
    int state = 0;
    if (!((chessBoard[move_Piece[3]][move_Piece[2]]>=65+g) && (chessBoard[move_Piece[3]][move_Piece[2]]<=90+g))){  // if the player kill his pieces don't continue by searching small and capital ASCII character
        if(abs(move_Piece[3]-move_Piece[1])==2 && abs(move_Piece[2]-move_Piece[0])==1){         // valid move if rows diff is 2 and col diff is 1
        state = 1;
        }else if(abs(move_Piece[3]-move_Piece[1])==1 && abs(move_Piece[2]-move_Piece[0])==2){   // valid move if rows diff is 1 and col diff is 2
        state = 1;
        }
    }else{
     state =0;                                              // invalid move
    }

    return state;
}
int checkBishobValidMove(int move_Piece[] , int size1,int turn , char chessBoard[][14]){
    int state=1;
    int i = move_Piece[0];              // From the current position to the required position check if the way is empty
    int j = move_Piece[1];
    while(move_Piece[2]!=i){
        if (move_Piece[2]-1>i){
            i++;
            if(move_Piece[3]-1>j){
                j++;
            }else if(move_Piece[3]+1<j){
                j--;
            }else{
                break;
            }
        }
         else if (move_Piece[2]+1<i){
            i--;
            if(move_Piece[3]-1>j){
                j++;
            }else if(move_Piece[3]+1<j){
                j--;
            }else{
                break;
            }
        }
        else
            break;
        if ((chessBoard[j][i]!='-'&& chessBoard[j][i]!='.'))
        {
            state =0;
            break;
        }
    }
    if(state){
    if(abs(move_Piece[3]-move_Piece[1])==abs(move_Piece[2]-move_Piece[0])){     // if absolute diff of rows =  absolute diff of col
        state = 1;          //Valid
    }else{
        state = 0;          //Invalid
    }
    }
    return state;
}
int enPassantCheck (int move_Piece[],int turn){
    if(turn%2==0){
        if(move_Piece[1]== 4 && move_Piece[2]==enPassant){     //for player 1 if the pawn in 5th row-4th in the chessboard- the pawn is going to the column of opponent pawn -who just moved 2 squares-
            enPassantPlayer=1;          // Player 1 made En passent
            return 1;
        }
    }else{
        if(move_Piece[1]== 5 && move_Piece[2]==enPassant){     //for player 2 if the pawn in 4th row-5th in the chessboard- the pawn is going to the column of opponent pawn -who just moved 2 squares-
            enPassantPlayer=2;         // Player 2 made En passent
            return 1;
        }
    }
    return 0;
}
int checkPawnValidMove(int move_Piece[],int size1,int turn,char chessBoard[][14]){
int player=0;
int state=0;
int g=0;
if(turn%2==0){
    g=0;
    player=-1;          // player 1 row diff is negative
}else{
    g=32;
    player=1;           // player 2 row diff is positive
}

if(move_Piece[3]-move_Piece[1]==1*player && abs(move_Piece[2]-move_Piece[0])==1){       // pawn move to kill if row diff is consistent with player and col diff is 1
    if ( (chessBoard[move_Piece[3]][move_Piece[2]]>=65+g && chessBoard[move_Piece[3]][move_Piece[2]]<=90+g)){   // if the move kills enemy piece
            state = 1;          // Valid move
        }else if(enPassantCheck(move_Piece,turn)){
            state = 1;          // En passant move
        }else{
            state = 0;          // invalid move
        }
}
else if (move_Piece[1]==7||move_Piece[1]==2)        // first move for pawn
{

    if((move_Piece[3]-move_Piece[1]==2*player ) && (move_Piece[2]-move_Piece[0]==0)){       // if the pawn move 2 squares
        if ((chessBoard[move_Piece[3]][move_Piece[0]]!='.' && chessBoard[move_Piece[3]][move_Piece[0]]!='-')){    // there is piece prevent the move in 2nd square
            state =0;
        }else{
            if((chessBoard[move_Piece[3]-1*player][move_Piece[0]]!='.' && chessBoard[move_Piece[3]-1*player][move_Piece[0]]!='-')){ // there is piece prevent the move in 1st square
            state = 0;
            }else{
                state = 1;                                      //Valid move
                enPassant = move_Piece[2];                      //Store the col number in enPassant
                enPassantFlag=-1;                               //En passant could be done in next turn
            }
        }
    }else if (move_Piece[3]-move_Piece[1]==1*player && (move_Piece[2]-move_Piece[0]==0)){           // if the pawn move 1 square
        if ((chessBoard[move_Piece[3]][move_Piece[0]]!='.' && chessBoard[move_Piece[3]][move_Piece[0]]!='-')){      // there is piece prevent the move in the square
            state =0;
        }else{
            state =1;               // Valid move
        }
    }
    else{
        state= 0;
    }
}
else{
    if(move_Piece[3]-move_Piece[1]==1*player && move_Piece[2]-move_Piece[0]==0){                    // if the pawn move 1 square
     if ((chessBoard[move_Piece[3]][move_Piece[0]]!='.' && chessBoard[move_Piece[3]][move_Piece[0]]!='-')){         // there is piece prevent the move in the square
            state =0;
        }else{
        state =1;                  // Valid move
        }
    }
    else{
        state= 0;
    }
}
return state;
}
int checkRookValidMove(int move_Piece[],int size1,char chessBoard[][14],int turn){
int g = 0;
int i = 0;
int state=1;
if(turn%2==0){
    g=32;
}else{
    g=0;
}
 if (move_Piece[3]-move_Piece[1]==0){       // Horizontal move
      i = move_Piece[0];
    while(move_Piece[2]!=i){                // from the current position until the final position the way is empty
          if ((chessBoard[move_Piece[3]][move_Piece[2]]>=65+g && chessBoard[move_Piece[3]][move_Piece[2]]<=90+g)){
            state =0;
            break;
        }
        if (move_Piece[2]-1>i)              // if rook moves to the right
            i++;
         else if (move_Piece[2]+1<i)        // if rook moves to the left
            i--;
        else
            break;

        if ((chessBoard[move_Piece[1]][i] !='-' && chessBoard[move_Piece[1]][i] !='.'))
        {
            state =0;
            break;
        }
    }
if (state)
    return 1;           //the way is empty and valid move
else
    return 0;

        }
else if (move_Piece[2]-move_Piece[0]==0){   // Vertical move
      i = move_Piece[1];
    while(move_Piece[3]!=i){                // from the current position until the final position the way is empty
        if ((chessBoard[move_Piece[3]][move_Piece[2]]>=65+g && chessBoard[move_Piece[3]][move_Piece[2]]<=90+g))
        {
            state =0;
            break;
        }
        if (move_Piece[3]-1>i)              // if rook moves to the down
            i++;
         else if (move_Piece[3]+1<i)        // if rook moves to the up
            i--;
        else
            break;

        if ((chessBoard[i][move_Piece[2]]!='-' && chessBoard[i][move_Piece[2]] !='.'))
        {
            state =0;
            break;
        }
    }
if (state)
    return 1;           //the way is empty and valid move
else
    return 0;
}
else
    return 0;

}
int checkQueenValidMove (int move_Piece[],int size1,int turn,char chessBoard[][14]){
    int state = 0;
        state = checkRookValidMove(move_Piece,size1,chessBoard,turn);       //Check if the queen moves like the rook
    if(state==0){
        state = checkBishobValidMove(move_Piece,size1,turn,chessBoard);     //Check if the queen moves like the bishop
    }
    return state;
}
int checkEmptyRow(int start,int End,int row,char chessBoard[][14]){
for (int i = start;i<=End;i++){
    if (chessBoard[row][i]!='-'&&chessBoard[row][i]!='.'){
        return 0 ;
    }
}
return 1;
}

int checkKingValidMove(int move_Piece[],int size1,int turn,char chessBoard[][14]){      // moves like bishob and rook for 1 square
    int state = 0;
    if(abs(move_Piece[3]-move_Piece[1])<=1 && abs(move_Piece[2]-move_Piece[0])<=1){
        state = checkRookValidMove(move_Piece,size1,chessBoard,turn);
         if(state==0){
            state = checkBishobValidMove(move_Piece,size1,turn,chessBoard);
    }
    }

    return state;
}

int checkForAcheckOnAking(char chessBoard[][14],int turn,int k1X,int k1Y,int k2X,int k2Y){
int state=0;
int move_Piece[4]={0};              // Declare array to store the different position
int g=0;
int v1=enPassant;                   // storing en passent data
int v2=enPassantFlag;
int v3=enPassantPlayer;
if(turn%2==0){                      // get the coordinates of player's king
    g=0;
    move_Piece[3] = k1X;
    move_Piece[2] = k1Y;
    }else{
    move_Piece[3] = k2X;
    move_Piece[2] = k2Y;
    g=32;
    }
for (int i=1;i<9;i++){                                  //make all pieces move towards the king
for (int j = 3;j<11;j++){
if (chessBoard[i][j]>65+g && chessBoard[i][j]<90+g){    //if the piece is from enemy pieces
move_Piece[1] = i;
move_Piece[0] = j;
switch(chessBoard[move_Piece[1]][move_Piece[0]]){       //check if it is valid move and kills the king
case 'p':
    state=checkPawnValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'P':
    state=checkPawnValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'r':
    state = checkRookValidMove(move_Piece,4,chessBoard,turn+1);
    break;
case 'R':
     state = checkRookValidMove(move_Piece,4,chessBoard,turn+1);
    break;
case 'b':
    state = checkBishobValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'B':
    state = checkBishobValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'n':
    state = checkKnightValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'N':
    state = checkKnightValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'k':
    state = checkKingValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'K':
    state = checkKingValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'q':
    state = checkQueenValidMove(move_Piece,4,turn+1,chessBoard);
    break;
case 'Q':
    state = checkQueenValidMove(move_Piece,4,turn+1,chessBoard);
    break;
default:
    state =0;
    break;
}

    }
if (state)                  // if at least 1 pieces could kill the king
    break;
    }
if (state)
    break;
}
enPassant= v1;              // retrieve en passant data
enPassantFlag = v2;
enPassantPlayer = v3;
if (state){


    if(turn%2==0){          // check for checked king
        checkUponKing=1;
    }else{
        checkUponKing=2;
    }
    return 1;
}else{
    return 0;
}
}
int checkCastling(int move_Piece[],char chessBoard[][14],int turn){
int state=0;
if (turn%2==0){                                     // check for player
   if (move_Piece[0]==7&&move_Piece[1]==8&&castlingValid[0]==0&&!checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,king2_x,king2_y)){     // if the king for player 1 was selected and has't been moved and it isn't in check continue
        chessBoard[8][7]='.';
        if (move_Piece[2]==9&&move_Piece[3]==8&& chessBoard[8][10]=='r' && castlingValid[1]==0 && checkEmptyRow(8,9,8,chessBoard) && !checkForAcheckOnAking(chessBoard,turn,8,9,king2_x,king2_y)&& !checkForAcheckOnAking(chessBoard,turn,8,8,king2_x,king2_y)){        // check if the right rook and hasn't been moved and there check in way of the king and the way between the king and the rook is empty
            for (int i=0 ; i<10;i++){                           // check if the castle is the true castle not another one
                for (int j=0; j<14;j++){
                    if ( i == 8 && j == 10 )
                        continue;
                    else if (chessBoard[i][j]=='r')
                    {
                        chessBoard[8][8]='r';
                        chessBoard[8][10]='-';
                        state=1;
                    }
                }
            }
        }
        else if (move_Piece[2]==5&&move_Piece[3]==8&& chessBoard[8][3]=='r'&& castlingValid[2]==0&& checkEmptyRow(4,6,8,chessBoard) && !checkForAcheckOnAking(chessBoard,turn,8,5,king2_x,king2_y)&& !checkForAcheckOnAking(chessBoard,turn,8,6,king2_x,king2_y)){    // check if the left rook and hasn't been moved and there check in way of the king and the way between the king and the rook is empty
             for (int i=0 ; i<10;i++){          // the same as previous
                for (int j=0; j<14;j++){
                    if ( i == 8 && j == 3 )
                        continue;
                    else if (chessBoard[i][j]=='r')
                    {
                       chessBoard[8][6]='r';
           	       chessBoard[8][3]='.';
                       state=1;
                    }
                }
            }
        }
        else{
        chessBoard[8][7]='k';
            state =0;
        }
   }
   else
   {
      state=0;
   }
}
else{           // same as previous
    if (move_Piece[0]==7&&move_Piece[1]==1&& castlingValid[3]==0&& !checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,king2_x,king2_y)) {
            chessBoard[1][7]='-';
       if (move_Piece[2]==9&&move_Piece[3]==1&& chessBoard[1][10]=='R'&& castlingValid[4]==0 && checkEmptyRow(8,9,1,chessBoard) && !checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,1,9)&&!checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,1,8)){
             for (int i=0 ; i<10;i++){
                for (int j=0; j<14;j++){
                    if ( i == 1 && j == 10 )
                        continue;
                    else if (chessBoard[i][j]=='R')
                    {
                          chessBoard[1][8]='R';
          		  chessBoard[1][10]='.';
          		  state=1;
                    }
                }
            }
            }
        else if (move_Piece[2]==5&&move_Piece[3]==1&& chessBoard[1][3]=='R'&& castlingValid[5]==0 && checkEmptyRow(4,6,1,chessBoard) && !checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,1,5)&&!checkForAcheckOnAking(chessBoard,turn,king1_x,king1_y,1,6)){
              for (int i=0 ; i<10;i++){
                for (int j=0; j<14;j++){
                    if ( i == 1 && j == 3 )
                        continue;
                    else if (chessBoard[i][j]=='R')
                    {
                          chessBoard[1][6]='R';
             		  chessBoard[1][3]='-';
           		  state = 1;
                    }
                }
            }
        }
        else{
           chessBoard[1][7]='K';
           state = 0;
        }

   }

   else{
       state = 0;
   }

}
return state;
}
int checkValidForDraw(char chessBoard[][14],int move_Piece[],int turn){     //check if there is any piece could move
int state=0;
int g=0;
if (turn%2==0)
    g=32;
else
    g=0;
switch(chessBoard[move_Piece[1]][move_Piece[0]]){
case 'p':
    state=checkPawnValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'P':
    state=checkPawnValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'r':
    state = checkRookValidMove(move_Piece,4,chessBoard,turn);
    break;
case 'R':
     state = checkRookValidMove(move_Piece,4,chessBoard,turn);
    break;
case 'b':
    state = checkBishobValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'B':
    state = checkBishobValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'n':
    state = checkKnightValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'N':
    state = checkKnightValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'k':
    state = checkKingValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'K':
    state = checkKingValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'q':
    state = checkQueenValidMove(move_Piece,4,turn,chessBoard);
    break;
case 'Q':
    state = checkQueenValidMove(move_Piece,4,turn,chessBoard);
    break;
default:
    state =0;
    break;
}
if (chessBoard[move_Piece[3]][move_Piece[2]]>65+g &&chessBoard[move_Piece[3]][move_Piece[2]]<90+g)      // check if the piece kill allay
        state = 0;      // Invalid move
return state;
}
int checkForAcheck(char chessBoard[][14],int move_Piece[],int turn){
    int checkIfAcheck=0;
    char chessBoard1[10][14];                           //declare an array equal to actual chessboard
    for(int i=0;i<10;i++){
    for(int j=0;j<14;j++){
        chessBoard1[i][j] = chessBoard[i][j];
    }
   }
   chessBoard1[move_Piece[3]][move_Piece[2]]= chessBoard1[move_Piece[1]][move_Piece[0]];    // apply the move in the new chessboard
    chessBoard1[move_Piece[1]][move_Piece[0]] ='-'; //empty the square of the move piece

   if(chessBoard1[move_Piece[3]][move_Piece[2]]=='k'){                                     // if the moved piece was the white king
       checkIfAcheck= checkForAcheckOnAking(chessBoard1,turn,move_Piece[3],move_Piece[2],king2_x,king2_y);    // check for a check in the new position of the king

   }else if(chessBoard1[move_Piece[3]][move_Piece[2]]=='K'){
       checkIfAcheck = checkForAcheckOnAking(chessBoard1,turn,king1_x,king1_y,move_Piece[3],move_Piece[2]);   // same as white king
   }
   else{
      checkIfAcheck= checkForAcheckOnAking(chessBoard1,turn,king1_x,king1_y,king2_x,king2_y);     // check for a check
   }
   return checkIfAcheck;
}
int checkForRemaningPieces(char piecesArr[],int n,int bishopIndexes[]){                     // check for special cases for draw
char invalidArrayA[] = {'q','Q','p','P','r','R'};                       // pieces if they were in the chessboard there is no draw yet
int counter [] = {0,0,0,0};                                             // declaration of array that count the number of knights and bishops
for (int i=0;i<6;i++){
    for (int j=0;j<n;j++)
        if (invalidArrayA[i]==piecesArr[j])                             // check if the board contain any invalid pieces for draw
    {
        return 0;
    }

}

for (int i=0;i<n;i++){
    switch(piecesArr[i]){         // count the number of knights and bishops and if a one is repeated more than once there is no draw
case 'n':
    if (counter[0]==1)
        return 0;
    else
        counter[0]++;
    break;
case 'b':
    if (counter[1]==1)
        return 0;
    else
        counter[1]++;
    break;
case 'N':
    if (counter[2]==1)
        return 0;
    else
        counter[2]++;
    break;
case 'B':
    if (counter[3]==1)
        return 0;
    else
        counter[3]++;
    break;
default:
    break;

    }
}
int sum=0;
for (int i=0;i<4;i++){            // sum the number of pieces
sum+=counter[i];
}
if (sum==2 && counter[1]==1 && counter[3]==1){                                                          // check if the remaining pieces are two bishops white and black of the same color
    if (bishopIndexes[0]%2==1&&bishopIndexes[1]%2==1 || bishopIndexes[0]%2==0&&bishopIndexes[1]%2==0){
        if (bishopIndexes[2]%2==1&&bishopIndexes[3]%2==1 || bishopIndexes[2]%2==0&&bishopIndexes[3]%2==0){
            return 1;
    }
    }
    else{
          if (bishopIndexes[2]%2==1&&bishopIndexes[3]%2==0 || bishopIndexes[2]%2==0&&bishopIndexes[3]%2==1)
                return 1;
    }
}
if (sum>1){                       // if the sum is less than one that means that (king vs king) or (king and bishop vs king) or (king and knight vs king)
return 0;
}
return 1;
}
int generateRandomMove (char chessBoard[][14],int x,int y,int turn){
int move_Piece[4]={0};
move_Piece[0]=y;            //coordinates for the piece
move_Piece[1]=x;
for (int i=1;i<=8;i++){             // generate random move for the piece
    for (int j =3; j<=10;j++){
        move_Piece[3]=i;
        move_Piece[2]=j;
        if (checkValidForDraw(chessBoard,move_Piece,turn)&&!checkForAcheck(chessBoard,move_Piece,turn)){      // check if move is valid and don't cause check
            if(i==x && j==y){       // ignore the same coordinate for the piece
                continue;
            }
            return 1;
        }
    }
}
return 0;
}
int drawState(char chessBoard[][14],int turn){      //function to detect if there is a draw or not
int g=0;
char Pieces[32]={'\0'};                // declare an array to store current pieces in the chessboard
int np=0;
int state=1;
int l=0;
int v1= enPassant;                     // store en passant data
int v2=enPassantFlag;
int v3=enPassantPlayer;
int bishopIndexes[4]={0};
if (turn%2==0){                        // determine white or black pieces
    g=32;
    l=0;
}
else{
    g=0;
    l=32;
}
for (int i=1;i<=8;i++){
    for (int j=3;j<=10;j++){
        if (chessBoard[i][j]>65+g && chessBoard[i][j]<90+g){
            if(generateRandomMove(chessBoard,i,j,turn)){        // if there is a valid random move of the current player
                state = 0;                                      // there is no draw
            }
            Pieces[np++] = chessBoard[i][j];                    // store the pieces
        }else if (chessBoard[i][j]>65+l && chessBoard[i][j]<90+l){

            Pieces[np++] = chessBoard[i][j];                    // store the pieces for the other player
        }
        if (chessBoard[i][j]=='b')
        {
            bishopIndexes[0]=i;                                             // store last found white and black bishop indexes
            bishopIndexes[1]=j;
        }
        else if (chessBoard[i][j]=='B')
        {
            bishopIndexes[2]=i;
            bishopIndexes[3]=j;
        }
    }
}
if (!state&&!winDrawSwitch){                                        // if is there was at least one move valid
   state= checkForRemaningPieces(Pieces,np,bishopIndexes);        // check for special cases
}
enPassant= v1;                                      // retrieve en passant data
enPassantFlag = v2;
enPassantPlayer = v3;
return state;
}
void movePiece(char chessBoard[][14],char inp[],int size1,int size2,int turn){
    int move_Piece[4];                                                      // declaration of array to store coordinates of current and required postion
    for (int i=0;i<4;i+=2){               // conversion of input array to coordinates
        move_Piece[i]=inp[i]-65+3;
        move_Piece[i+1]=9-(inp[i+1]-49+1);
    }
    int state=1;
   if (turn%2==0){  // check if the player selects a valid piece and does not killing his allay
    if (chessBoard[move_Piece[1]][move_Piece[0]]>97 &&chessBoard[move_Piece[1]][move_Piece[0]]<122){
        if (!(chessBoard[move_Piece[3]][move_Piece[2]]>97 &&chessBoard[move_Piece[3]][move_Piece[2]]<122)){
        state = 1;
    }else{
        state = 0;
        playerTurn=0;
    }
    }
    else{
    state =0;
    playerTurn =0;
   }
   }
   else if (turn%2==1 ){ // the same but for player 2
    if (chessBoard[move_Piece[1]][move_Piece[0]]>65 &&chessBoard[move_Piece[1]][move_Piece[0]]<90){
        if (!(chessBoard[move_Piece[3]][move_Piece[2]]>65 &&chessBoard[move_Piece[3]][move_Piece[2]]<90)){
        state = 1;
    }else{
        state = 0;
        playerTurn=0;
    }
    }
    else{
    state =0;
    playerTurn =0;
    }
   }
   int checkIfAcheck = 0;  // indicator if the move cause a check for the current players' king and it must not be 1 to be a valid game
   if(state){
    checkIfAcheck = checkForAcheck(chessBoard,move_Piece,turn);
    if(checkIfAcheck){
        state = 0;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Be Careful !","There is a check",NULL);
        playerTurn=0;
    }else{
        state = 1;
        playerTurn=1;
    }
   }
int checkValidCastling=0;
   if (checkCastling(move_Piece,chessBoard,turn)&& state){          // check if there is a castling
    checkValidCastling=1;
    printf("castling has been made\n");
    if (turn%2==0){     // storing the new coordinates of the king "very important in checking for check"
    king1_x = move_Piece[3];
    king1_y = move_Piece[2];
    chessBoard[move_Piece[3]][move_Piece[2]]='k';
    castlingValid[0]=1;   // no castling can be done for this king
    }
    else {
    king2_x = move_Piece[3];        // the same as previous
    king2_y = move_Piece[2];
    chessBoard[move_Piece[3]][move_Piece[2]]='K';
    castlingValid[3]=1;
    }
    state=0;
    playerTurn=1;
   }

if (state){
switch(chessBoard[move_Piece[1]][move_Piece[0]]){   // check if the move is valid for the selected piece
case 'p':
    state=checkPawnValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;

    break;
case 'P':
    state=checkPawnValidMove(move_Piece,4,turn,chessBoard);  // check for the same conditions as previous
    playerTurn=state;

    break;
case 'r':
    state = checkRookValidMove(move_Piece,4,chessBoard,turn);
    playerTurn=state;
    if (state && move_Piece[0]==10) // if it was valid move make indicator that the rock has been moved
        castlingValid[1]=1;
    else if (state)
        castlingValid[2]=1;

    break;
case 'R':
     state = checkRookValidMove(move_Piece,4,chessBoard,turn); // the same as previous
     playerTurn=state;
      if (state && move_Piece[0]==10)
        castlingValid[4]=1;
    else if (state)
        castlingValid[5]=1;

    break;
case 'b':
    state = checkBishobValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;

case 'B':
    state = checkBishobValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;
case 'n':
    state = checkKnightValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;
case 'N':
    state = checkKnightValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;
case 'k':
    state = checkKingValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
 if (state){                                                    // if it was a valid move get the new coordinates of the king and make any castling with this king invalid
    castlingValid[0]=1;
    king1_x = move_Piece[3];
    king1_y = move_Piece[2];
    }
    break;
case 'K':
    state = checkKingValidMove(move_Piece,4,turn,chessBoard);  // the same as prevoius
    playerTurn=state;
    if (state){
        castlingValid[3]=1;
        king2_x = move_Piece[3];
        king2_y = move_Piece[2];
    }
    break;
case 'q':
    state = checkQueenValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;
case 'Q':
    state = checkQueenValidMove(move_Piece,4,turn,chessBoard);
    playerTurn=state;
    break;
default:
    state =0;
    playerTurn=state;
    break;
    }
}
if(enPassantFlag>=0){ // flag to make enpassant invalid if it was not made directly
    enPassantFlag=0;
}
enPassantFlag++;

    if (state){
    if (chessBoard[move_Piece[3]][move_Piece[2]]>65 &&chessBoard[move_Piece[3]][move_Piece[2]]<90)      // check if the player kill an enemy
       {

        if (player2_x==9){ // if the variable reached the last row start from the next cloumn
            player2_x=0;
            player2_y=0;
        }
        chessBoard[player2_x++][player2_y]= chessBoard[move_Piece[3]][move_Piece[2]]; // fill chessboard's side cloumns array with the removed pieces

    }
    else if (chessBoard[move_Piece[3]][move_Piece[2]]>65+32 &&chessBoard[move_Piece[3]][move_Piece[2]]<90+32){ // the same but for the removed pieces of the other player
         if (player1_x==9){
            player1_x=0;
            player1_y=13;
        }
        chessBoard[player1_x++][player1_y]= chessBoard[move_Piece[3]][move_Piece[2]];
    }else if(enPassantPlayer!=0){ // enPassant player is 1 for player 1 and so on but it's zero if no player has played enpassant special move
        printf("ENPASSANT !\n");
        if(enPassantPlayer==1){ // if player 1 has made enpassant
            if (player2_x==9){
            player2_x=0;
            player2_y=0;
        }
        chessBoard[player2_x++][player2_y]= 'P';    // fill the side column with P
        if(chessBoard[3][enPassant]=='.'){          // empty the square of the removed piece
            chessBoard[4][enPassant]='-';

        }else{
            chessBoard[4][enPassant]='.';

        }
        }else{  // the same but for player 2
            if (player1_x==9){
            player1_x=0;
            player1_y=13;
        }
        chessBoard[player1_x++][player1_y]= 'p';
        if(chessBoard[6][enPassant]=='.'){
            chessBoard[5][enPassant]='-';
        }else{
            chessBoard[5][enPassant]='.';
        }

        }
        enPassantPlayer=0;
        enPassant=-1;
    }
    if (inp[4]!='\0'){
        chessBoard[move_Piece[3]][move_Piece[2]]= inp[4];
    }
    else {
    chessBoard[move_Piece[3]][move_Piece[2]]= chessBoard[move_Piece[1]][move_Piece[0]];         // move the piece to the selected square
    }

    if (move_Piece[1]%2==1){  // empty the square of the moved piece by determination of black and white squares relative to the relation between columns and rows

        if (move_Piece[0]%2==1)
            chessBoard[move_Piece[1]][move_Piece[0]] ='-';
        else
            chessBoard[move_Piece[1]][move_Piece[0]] ='.';
    }
    else {
         if (move_Piece[0]%2==0)
            chessBoard[move_Piece[1]][move_Piece[0]] ='-';
        else
            chessBoard[move_Piece[1]][move_Piece[0]] ='.';
    }

    }
    else if (!state&&!checkValidCastling)
        printf("Invalid Move !\n");
    if (enPassantFlag>=1)                   // make the enpassant move invalid because it has not been made directly
    {
        enPassant=-1;
        enPassantPlayer=0;
    }
}
void saveData(char inp[5]){

    if (uCounter!=-1){      // if there is an undo has been made the variable value will not be -1
    FILE *file;
    char str[uCounter*5];                   // array to store the characters to take from the first to ignore the undo data "uCounter equals the number of groups of 5 characters"
    file = fopen("ChessData.txt","r");
    if ( file== NULL )
    {

    }
    else
    {
       int k = uCounter*5;         // store the number characters with ignoring undo characters
       int i=0;
       while(k>0){                          // store the characters in the array
        str[i++]=fgetc(file);
        k--;
       }
        fclose(file);                   // close the file
        remove("ChessData.txt");                // remove it
        FILE *file1;
        file1= fopen("ChessData.txt","a");          // create a file with the same name to append data
        for (int j=0;j<i;j++)
        fprintf(file1,"%c",str[j]);             // append the data stored in str array that contains data without undo inputs
        fclose(file1);
        uCounter=-1;                            // make the variables as if there was not any undo before
        fileCurrentSize=0;
    }
    }
    FILE *file2;
    file2 = fopen("ChessData.txt","a");             // open the file for appending
    if ( file2== NULL )
    {

    }
    else
    {

    for (int i=0;i<5;i++)                       // save the data to the running game file
        fprintf(file2,"%c",inp[i]);
    uCounter=-1;
    fileCurrentSize=0;

   }
        fclose(file2);

}
void readData(char chessBoard[][14],int ur ){
if (!(uCounter==fileCurrentSize && ur==0) && !(uCounter==-1&& ur==0)){
player1_x=0;                                                              // retrieving the global variables initial values
player1_y=12;
player2_x=0;
player2_y=1;
playerTurn=1;
king1_x=8;
king1_y=7;
king2_x=1;
king2_y=7;
checkUponKing=0;
for (int i=0;i<6;i++)
    castlingValid[i]=0;
enPassant= -1;
enPassantFlag = 0;
enPassantPlayer = 0;
char chessBoard1[10][14]={
     {' ',' ',' ','A','B','C','D','E','F','G','H',' ',' ',' '},
    {' ',' ','8','R','N','B','Q','K','B','N','R','8',' ',' '},
    {' ',' ','7','P','P','P','P','P','P','P','P','7',' ',' '},
    {' ',' ','6','-','.','-','.','-','.','-','.','6',' ',' '},
    {' ',' ','5','.','-','.','-','.','-','.','-','5',' ',' '},
    {' ',' ','4','-','.','-','.','-','.','-','.','4',' ',' '},
    {' ',' ','3','.','-','.','-','.','-','.','-','3',' ',' '},
    {' ',' ','2','p','p','p','p','p','p','p','p','2',' ',' '},
    {' ',' ','1','r','n','b','q','k','b','n','r','1',' ',' '},
    {' ',' ',' ','A','B','C','D','E','F','G','H',' ',' ',' '}
    };
for (int i=0;i<10;i++)                       // make the current board equal to the initial board
    for(int j=0;j<14;j++)
        chessBoard[i][j]=chessBoard1[i][j];
FILE *file;                                              // declaration of file pointer
file = fopen("ChessData.txt","r");                      // open the file for reading only in this function it's required for restoring data to implement undo and redo functions

    if ( file== NULL )
    {
        printf("Error! no Data can be loaded\n");
        turn =0;
    }
    else
    {
    int k=0;

    if (uCounter==-1){                  // if it was -1 then it is the first calling for undo
    fseek(file,0L,SEEK_END);
   uCounter=ftell(file)/5;              // get the number of 5-characters groups in the file
   fileCurrentSize=uCounter;
   fseek(file,0L,SEEK_SET);
    }
    if (uCounter==0&&ur==1){                    // if the user is choosing undo but it is the initial chessboard
        printf("No prevoius move are found\n");
    }
   if (uCounter!=0 && ur==1){  //if the number of input groups is not zero and the user choose undo get the number of inputs from the first till the previous one
    uCounter--;
    k=uCounter;  // assign the value to var k
   }
   else if (ur==0&&uCounter<fileCurrentSize){       // if ur = 0 the user choose redo now check if uCounter is less than the number of inputs in the total file to make redo
   uCounter++;
   k=uCounter;          // assign the value to var k
   }
    turn =0;
   while(k>0){          // repeat the loop from the first input to specified input
    char inp[5]={'\0'};
    for (int j=0;j<5;j++){
        inp[j]=fgetc(file);                 // to get the input from the file
    }
        if(drawState(chessBoard,turn)){
        k--;
        break;
        }
    if(!((inp[0]>=65 && inp[0]<=72) &&(inp[2]>=65 && inp[2]<=72)) ){
       uCounter = turn-1;                                                                   // to read till the first incorrect input if there is error in the file
        break;
    }
    if(!((inp[1]>=49 && inp[1]<=56) &&(inp[3]>=49 && inp[3]<=56)) ){
    uCounter = turn-1;
    break;
    }
    movePiece(chessBoard,inp,10,4,turn%2);
    if(checkForAcheckOnAking(chessBoard,turn+1,king1_x,king1_y,king2_x,king2_y)){
            winDrawSwitch=1;
    if (drawState(chessBoard,turn+1)){
        k--;
        break;
    }
    }
    winDrawSwitch=0;
    if (playerTurn==0){
    playerTurn=1;
    k--;
    uCounter = turn-1;
    break;
    }
    k--;
    turn++;
    }
   }
        fclose(file);    // closing the file
}
}
void copyToFile(char name1[],char name2[],int state){     // function to copy data from file to another
FILE*file1;
file1 = fopen(name1,"r");           // open file to read from it
if (file1!=NULL){
FILE*file2;
file2 = fopen(name2,"w");               // open file to copy to it
if (state && uCounter!=-1){
int k=uCounter*5;
while(k>0){
int ch=fgetc(file1);
fputc(ch,file2);
k--;
}
}
else{
int ch = fgetc(file1);
while(ch!=EOF){                     // loop as soon as the we have not reached the end of the file
fputc(ch,file2);
ch=fgetc(file1);                        // copy character by character
}
}
fclose(file2);                  // close the files
}
fclose(file1);

}
void restoreLastSavedDAta(char chessBoard[][14]){           // function to apply data from a file to the chessboard
copyToFile(f2,f1,0);
FILE*file2;
file2=fopen(f2,"r");
if (file2!=NULL){
FILE*file1;
file1 = fopen(f1,"r");                                  // make the file read only
fseek(file1,0L,SEEK_END);
int Counter=ftell(file1)/5;                                         // get the size of the file
fseek(file1,0,SEEK_SET);
fileCurrentSize=Counter;
fclose(file1);
fclose(file2);                                             //close the file
uCounter=0;
for (int i=0;i<Counter;i++)                                 // call redo function to apply the inputs from the first one saved to the last one
    readData(chessBoard,0);
uCounter=-1;
fileCurrentSize=0;
}
else
    fclose(file2);
}
void printArray(char chessBoard[][14],int size1,int size2){  // function to print the array
  for (int i=0;i<size1;i++){
        for (int j=0;j<size2;j++){
                printf(" %c ",chessBoard[i][j]);
        }
        printf("\n");
        }
}
void getInputFromMouse(char inp[],int x,int y,int m){

    if (x>722 && x<775 && y >26 && y < 81){
        inp[0]= 'u';
        m=m+2;
        ignore=1;

    }else if (x>722 && x<775 && y >136 && y < 191){
        inp[0]= 'r';
        m=m+2;
        ignore=1;
    }
      else if (x>722 && x<775 && y >246 && y < 301){
        inp[0]= 'l';
        m=m+2;
        ignore=1;
    }
      else if (x>722 && x<775 && y >356 && y < 411){
        inp[0]= 's';
        m=m+2;
        ignore=1;
    }
if (!ignore){
for (int i=0;i<8;i++){
    if (x>126+i*55&&x<181+i*55){
        inp[m%4]='A'+i;
        break;
        }
}
for (int i=0;i<8;i++){
    if ((y>(26+i*55))&&(y<(81+i*55))){
        inp[(m+1)%4]='8'-i;
        break;
        }
}
}
}
void zeroArray(char input[],int size1){
for (int i=0;i<size1;i++)
    input[i]='\0';
}
int main(int argc, char ** argv)
{
    remove("ChessData.txt");
    int quit=0;                                 // to exit from while loop
    SDL_Event event;                            // to handle all events
    SDL_Init(SDL_INIT_VIDEO);                   // to initialize SDL library
    int invalid =0;
    SDL_Window * window = SDL_CreateWindow("Chess Game",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,830,500, 0);               // create a window
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);                            // create 2D render
    SDL_Surface * image = SDL_LoadBMP("C1.bmp");                            // load the background image
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,image);      // convert SDL_surface to SDL_texture because the render only renders textures
    char input[5]={'\0'};
    char chessBoard[10][14]={
   {' ',' ',' ','A','B','C','D','E','F','G','H',' ',' ',' '},
    {' ',' ','8','R','N','B','Q','K','B','N','R','8',' ',' '},
    {' ',' ','7','P','P','P','P','P','P','P','P','7',' ',' '},
    {' ',' ','6','-','.','-','.','-','.','-','.','6',' ',' '},
    {' ',' ','5','.','-','.','-','.','-','.','-','5',' ',' '},
    {' ',' ','4','-','.','-','.','-','.','-','.','4',' ',' '},
    {' ',' ','3','.','-','.','-','.','-','.','-','3',' ',' '},
    {' ',' ','2','p','p','p','p','p','p','p','p','2',' ',' '},
    {' ',' ','1','r','n','b','q','k','b','n','r','1',' ',' '},
    {' ',' ',' ','A','B','C','D','E','F','G','H',' ',' ',' '}
    };
    SDL_Surface * image1;                                           // declaration of SDL_surface to load pieces and other pictures, SDL_texture to which we convert declared SDL_surface
    SDL_Texture * texture1;
    SDL_Rect image_1_rect;                                          // SDL_rect structure to stretch the image for specific size on the window
int k=0,xMouse=0,yMouse=0,m=0,winOrDraw=0;
SDL_RenderCopy(renderer, texture, NULL, NULL);                          // render the background
    void printGUI (int startX, int startY , int endX , int endY , int xShift , int yShift , int xSub , int ySub){           // function to render all the pieces found in the chessBoard
   for (int i=startX;i<endX;i++)
    {
        for (int j=startY;j<endY;j++)
        {
            switch(chessBoard[i][j]){
            case 'p':
               image1  = SDL_LoadBMP("P1.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'P':
               image1 = SDL_LoadBMP("P2.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'r':
               image1 = SDL_LoadBMP("R1.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'R':
              image1 = SDL_LoadBMP("R2.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'n':
              image1 = SDL_LoadBMP("N1.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'N':
               image1 = SDL_LoadBMP("N2.bmp");
               texture1  = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'b':
              image1 = SDL_LoadBMP("B1.bmp");
               texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'B':
               image1= SDL_LoadBMP("B2.bmp");
               texture1= SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'q':
               image1= SDL_LoadBMP("Q1.bmp");
               texture1= SDL_CreateTextureFromSurface(renderer, image1 );
               break;
            case 'Q':
               image1= SDL_LoadBMP("Q2.bmp");
               texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
               break;
             case 'k':
               image1= SDL_LoadBMP("K1.bmp");
               texture1 =SDL_CreateTextureFromSurface(renderer, image1 );
               break;
             case 'K':
              image1= SDL_LoadBMP("K2.bmp");
              texture1= SDL_CreateTextureFromSurface(renderer, image1 );
               break;
             default :
                 k=1;
                break;
            }
            if(k==1){
                k=0;
                continue;
            }
    image_1_rect.x=xShift+55*(j-xSub);                          // the start position and the size of each image of the different pieces
    image_1_rect.y=yShift+55*(i-ySub);
    image_1_rect.w=55;
    image_1_rect.h=55;

    SDL_RenderCopy(renderer, texture1 , NULL, &image_1_rect );                  // render pieces image

        }
    }
}
int promotionCheck(char chessBoard[][14],int*x , int*y, int*g){             // function to check if there is a not promoted pawn and if there is it ask the user to choose a apiece and return the index selected on
*x=0;
for (int j=3;j<11;j++)                  // check for the piece
{
    if (chessBoard[1][j]=='p')
    {
        *x=1;
        *y=j;
        *g=32;
        break;
    }
    else if (chessBoard[8][j]=='P')
        {
        *x=8;
        *y=j;
        *g=0;
        break;
    }

}
int p =-1;
if (*x!=0){             // if a not promoted pawn was foun

    image1= SDL_LoadBMP("promotion.bmp");                               // load screen to ask the user to choose anew piece
    texture1= SDL_CreateTextureFromSurface(renderer, image1 );
    image_1_rect.x=90+126;
    image_1_rect.y=210;
    image_1_rect.w=320;
    image_1_rect.h=80;
    SDL_RenderCopy(renderer, texture1 , NULL, &image_1_rect );                      //render the new pieces screen
    SDL_RenderPresent(renderer);                                                    //  update the screen
    int state = 1;
    while(state){
        SDL_WaitEvent(&event);                                                      // wait for an event
        if (event.type ==SDL_MOUSEBUTTONDOWN)                                       // if the mouse button was pressed
        {
            xMouse = event.motion.x;                                                // get the coordinates of the mouse pointer at the instant of pressing
            yMouse = event.motion.y;
            if (yMouse>210 && yMouse < 290){
                for (int i=0;i<4;i++ ){
                    if (xMouse>(216+80*i) && xMouse < (296+ 80*i)){                     // find the selected piece
                        p = i;
                        break;
                    }
                }
            }


        }
        if (p!=-1)
            state =0;
    }
}
return p;
}
    if (turn%2==0){                                                 //  displaying player turn
            image1 = SDL_LoadBMP("turn1.bmp");
                texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
         }
         else{
            image1 = SDL_LoadBMP("turn2.bmp");
                texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
         }
    image_1_rect.x=720;
    image_1_rect.y=445;
    image_1_rect.w=110;
    image_1_rect.h=55;
    SDL_RenderCopy(renderer, texture1 , NULL, &image_1_rect );
   printGUI(1,3,9,11,138,26,3,1);                                               // render the pieces that were found on the chessboard array
    int xNewPiece = 0 , yNewPiece = 0, gNewPiece =0 ;
    while (!quit)
    {

      SDL_WaitEvent(&event);                                                // wait for an event
        switch(event.type)
       {
        case SDL_MOUSEBUTTONDOWN:                                               //      if the mouse was pressed
    xMouse= event.motion.x;                                                        // get the coordinates of the mouse at instant of pressing
    yMouse = event.motion.y;
    getInputFromMouse(input,xMouse,yMouse,m);                                       // convert the coordinates to the input
    int arrayForLag[2];
       arrayForLag[0]=input[0]-65+3;
        arrayForLag[1]=9-(input[1]-49+1);
    if (chessBoard[arrayForLag[1]][arrayForLag[0]]=='-' || chessBoard[arrayForLag[1]][arrayForLag[0]]=='.'){
        m=0;
        zeroArray(input,5);
        break;
    }

         m+=2;
         if (m%4==0||ignore){               // ignore variable so that if the user choose undo,redo,load,save the program will ignore the remaining conditions
                for (int j=0;j<5;j++){
                    printf("%c",input[j]);              // defined in the console-version
                }
                printf("\n");

    for (int i=0;i<10;i++){
        for (int j=0;j<14;j++){
            printf(" %c ",chessBoard[i][j]);
        }
        printf("\n");
    }
    printf("Player %d turn\n",turn%2+1);
    char inp[5]={'\0'};
    for (int i=0;i<5;i++)
        inp[i]=input[i];
    if (inp[0]=='u'){

      readData(chessBoard,1);

    }else if (inp[0]=='r'){
         readData(chessBoard,0);

    }
      else if (inp[0]=='s'){
        if (uCounter!=0 && turn!=0)
         copyToFile(f1,f2,1);


      }
      else if (inp[0]=='l'){
            restoreLastSavedDAta(chessBoard);

      }
      if (!ignore){

    if(!((inp[0]>=65 && inp[0]<=72) &&(inp[2]>=65 && inp[2]<=72)) ){
        printf("Invalid Move !\n");
    zeroArray(input,5);
    m=0;
        break;
    }
    if(!((inp[1]>=49 && inp[1]<=56) &&(inp[3]>=49 && inp[3]<=56)) ){
        printf("Invalid Move !\n");
         zeroArray(input,5);
    m=0;
        break;
    }
    printf("You played! : ");
    for (int i=0;i<4;i++)
        printf("%c",inp[i]);
    printf("\n");
    movePiece(chessBoard,inp,10,4,turn%2);
    int newPiece = promotionCheck(chessBoard,&xNewPiece,&yNewPiece,&gNewPiece);
    if (newPiece!=-1){                                                           // there is no promotion
        if (newPiece==0){
        chessBoard[xNewPiece][yNewPiece]='Q'+gNewPiece;                                     // convert the pawn to what was selected
        inp[4]=chessBoard[xNewPiece][yNewPiece];
        }
        else if (newPiece==1){
        chessBoard[xNewPiece][yNewPiece]='B'+gNewPiece;
         inp[4]=chessBoard[xNewPiece][yNewPiece];
        }
        else if (newPiece==2){
       chessBoard[xNewPiece][yNewPiece]='N'+gNewPiece;
        inp[4]=chessBoard[xNewPiece][yNewPiece];
        }
        else if (newPiece==3){
        chessBoard[xNewPiece][yNewPiece]='R'+gNewPiece;
         inp[4]=chessBoard[xNewPiece][yNewPiece];
        }
        }
    newPiece = 0;
    if(checkForAcheckOnAking(chessBoard,turn+1,king1_x,king1_y,king2_x,king2_y)){
      winDrawSwitch=1;
    if (drawState(chessBoard,turn+1)){
            printf("CHECKMATE !\a\n");
            printf("GAME OVER BY WIN !\n");
        printArray(chessBoard,10,14);
        printf("PLAYER %d WIN :D\n",turn%2+1);
    quit = 1;
    winOrDraw=1;                                                    // if there is win and quit the program ans set a value for winOrDraw to render the image in the last of the program and close it
    }
    else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Warning","You checked the opponent's king good :)",NULL);              // show a message for check
    }
    winDrawSwitch=0;
    if (!winOrDraw){
    if(drawState(chessBoard,turn+1) ){
                printf("GAME OVER BY DRAW !\n");
        for (int i=0;i<10;i++){
        for (int j=0;j<14;j++){
                printf(" %c ",chessBoard[i][j]);
        }
        printf("\n");
        }
    quit = 1;                           // if there is quit the program
    winOrDraw=2;
        }
    }
    if (playerTurn==0){
    playerTurn=1;
    m=0;
    xMouse=0;
    yMouse=0;
    for (int i=0;i<5;i++)
      input[i]='\0';
    break;
    }
    saveData(inp);
    turn++;
      }
    ignore=0;
    m=0;
         }

         SDL_RenderCopy(renderer, texture, NULL, NULL);
         if (turn%2==0){
            image1 = SDL_LoadBMP("turn1.bmp");
                texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
         }
         else{
            image1 = SDL_LoadBMP("turn2.bmp");
                texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
         }
    image_1_rect.x=720;
    image_1_rect.y=445;
    image_1_rect.w=110;
    image_1_rect.h=55;
    SDL_RenderCopy(renderer, texture1 , NULL, &image_1_rect );

        printGUI(1,3,9,11,138,26,3,1);
        printGUI(0,0,9,2,0,0,0,0);
        printGUI(0,12,9,14,610,0,12,0);

       break;
         case SDL_QUIT:
           quit = 1;
           break;
       }

        if (winOrDraw==1 || winOrDraw==2){          // check if there is win or draw and render the corresponding image
        if (winOrDraw==1){
            if (turn%2+1==2){
             image1 = SDL_LoadBMP("p1win.bmp");
                texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
        }
        else{
            image1 = SDL_LoadBMP("p2win.bmp");
            texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
        }

        }
        else if (winOrDraw==2){
            image1 = SDL_LoadBMP("Draw.bmp");
            texture1 = SDL_CreateTextureFromSurface(renderer, image1 );
        }
    image_1_rect.x=235;
    image_1_rect.y=187.5;
    image_1_rect.w=250;
    image_1_rect.h=125;
    SDL_RenderCopy(renderer, texture1 , NULL, &image_1_rect );
    }

   SDL_RenderPresent(renderer);                 // update the screen with all renders
    if (quit&&winOrDraw){
        SDL_Delay(3000);
    }


    }

    SDL_DestroyTexture(texture);                // free all declared surfaces and textures
    SDL_FreeSurface(image);
    SDL_DestroyTexture(texture1);
    SDL_FreeSurface(image1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    for (int i=0;i<4;i++){
        printf("%c",input[i]);
    }
    return 0;
}

