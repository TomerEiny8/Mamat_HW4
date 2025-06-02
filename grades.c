#include "grades.h"
#include <stdlib.h>
#include <string.h>
#include "linked-list.h"

typedef struct course {
	char* name;
	int grade;
} *Course;

typedef struct student {
	char* name;
	int id;
	struct list *courses;
} *Student;

struct grades {
	struct list *students;
};


