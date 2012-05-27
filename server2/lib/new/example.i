%module example

%{
extern int get_size();
extern void get_path(double *);
extern int path_size;
%}	

extern int path_size;
extern int get_size();
extern void get_path(double *);

