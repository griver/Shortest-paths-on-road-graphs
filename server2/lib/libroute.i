%module libroute

%include "carrays.i"
%array_functions(double, doubleArray);

%{
extern int get_size();
extern void get_path(double *);
extern int path_size;
%}

extern int path_size;
extern int get_size();
extern void get_path(double *);

