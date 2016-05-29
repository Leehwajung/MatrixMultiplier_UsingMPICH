# Matrix Multiplier Using MPICH
Parallel Processing Course - MPI(MPICH) Project [Team Project]

## Command Line Options
### Usage
matrixmul [options]
### options
-p <matrix>   Print the matrix<br>
              __Allowed matrices:__<br>
              Input matrix A: a or 0<br>
              Input matrix B: b or 1<br>
              Result matrix C: c or 2<br>
              Whole matrices: w, 3 or any character<br>
-s <size>     Width of square matrices (Neutralize -h, -c and -w options)<br>
-h <size>     Height of input matrix A and result matrix C<br>
-c <size>     Width of input matrix A and height of input matrix B<br>
              (Correspondence of input matrices)<br>
-w <size>     Width of input matrix C and result matrix C<br>
-d            debug

## Contributors
Lee Hwajung<br>
Yang Sungmin<br>
Song Myeongho

## Developing Environment
Project Type: Win32 Console Application<br>
Language: C++<br>
IDE: Microsoft Visual Studio 2015 (v140)

## Environment Variable
### MPICH2
Installation path for MPICH2<br>
e.g. C:\Program Files (x86)\MPICH2
