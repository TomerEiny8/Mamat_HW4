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

/** USER FUNCTION for linked-list.h **/

/**
 * @brief Initializes a Course object with
 *      .name = "name", .grade = "grade"
 * @param name      The course name
 * @param grade     The course grade
 * @return          A new Course (i.e. struct course*), or NULL on failure
 */
static Course course_create(const char *name, int grade) {
    if (!name || grade < 0 || grade > 100) {
        return NULL;
    }

    Course new_course = malloc(sizeof(*new_course));
    if (!new_course) {
        return NULL;
    }

    new_course->name = malloc(strlen(name) + 1);
    if (!new_course->name) {
        free(new_course);
        return NULL;
    }
    strcpy(new_course->name, name);

    new_course->grade = grade;

    return new_course;
}

/**
 * @brief Clones a Course object
 * @param element An original Course object to clone
 * @param output A pointer to the cloned Course object
 * @returns 0 on success
 * @note On error, sets "output" to NULL.
 */
static int course_clone(void *element, void **output) {
	if (element == NULL || output == NULL) {
		*output = NULL;
		return 1;
		}

		Course orig = (Course) element;
		Course clone = malloc(sizeof(*clone));
		if (clone == NULL) {
			*output = NULL;
			return 1;
		}

		/** copy name of Course **/
		clone->name = malloc(strlen(orig->name) + 1);
			if (!clone->name){
				free(clone);
				*output = NULL;
				return 1;
			}
		strcpy(clone->name, orig->name);

		/** copy grade of Course **/
		clone->grade = orig->grade;

		*output = clone;
		return 0;
}

/**
 * @brief Destroys a Course object
 * @param element An original Course object to Destroy
 * @returns No return value
 */
static void course_destroy(void *element) {
	if (element != NULL) {
		Course elem = (Course) element;

		free(elem->name);
		free(elem);
	}
}

/**
 * @brief Clones a Student object
 * @param element An original Student object to clone
 * @param output A pointer to the cloned Student object
 * @returns 0 on success
 * @note On error, sets "output" to NULL.
 */
static int student_clone(void *element, void **output) {
	if (element == NULL || output == NULL) {
		*output = NULL;
		return 1;
	}

	Student orig = (Student) element;
	Student clone = malloc(sizeof(*clone));
	if (clone == NULL) {
		*output = NULL;
		return 1;
	}

	/** copy name of student **/
	clone->name = malloc(strlen(orig->name) + 1);
		if (!clone->name){
			free(clone);
			*output = NULL;
			return 1;
		}
	strcpy(clone->name, orig->name);

	/** copy id of student **/
	clone->id = orig->id;

	/** create *courses of clone **/
	clone->courses = list_init(course_clone, course_destroy);
	if (clone->courses == NULL) {
		free(clone->name);
		free(clone);
		*output = NULL;
		return 1;
	}

	/** copy courses using iterator **/
	for (struct iterator *it = list_begin(orig->courses);
			it != NULL;
			it = list_next(it)) {
		if (list_push_back(clone->courses, list_get(it)) != 0) {
			list_destroy(clone->courses);
			free(clone->name);
			free(clone);
			*output = NULL;
			return 1;
		}
	}

	*output = clone;
	return 0;
}

/**
 * @brief Destroys a Student object
 * @param element An original Student object to Destroy
 * @returns No return value
 */
static void student_destroy(void *element) {
	if (element != NULL) {
		Student elem = (Student) element;

		free(elem->name);
		list_destroy(elem->courses);
		free(elem);
	}
}


/** HELPER FUNCTUINS **/

/**
 * @brief Find first student-node with .id that matches "id"
 * @param students 	A linked-list of student objects
 * @param id 		The student id to search for
 * @return 			An Iterator pointing to the student-node if found
 * 					NULL if no matching student-node was found
 */
static struct iterator* grades_search(struct list *students, int id) {
	if (!students) {
		return NULL;
	}
	Student curr;
	for (struct iterator *it = list_begin(students);
				it != NULL;
				it = list_next(it)) {
		curr = (Student)list_get(it);
		if (curr->id == id) {
			return it;
		}
	}
	return NULL;
}

/**
 * @brief Initializes a Student object with
 * 		.name = "name", .id = "id", .courses = an empty list
 * @param name	 	The student's name
 * @param id 		The student's id
 * @return 			A new Student (i.e. struct student*), or NULL on failure
 */
static Student student_create(const char *name, int id) {
	if (!name) {
		return NULL;
	}

	Student new_stu = malloc(sizeof(*new_stu));
	if (!new_stu) {
		return NULL;
	}

	/** init student's name && id **/
	new_stu->name = malloc(strlen(name) + 1);
		if (!new_stu->name){
			free(new_stu);
			return NULL;
		}
	strcpy(new_stu->name, name);

	new_stu->id = id;

	/** init student's courses (empty list) **/
	new_stu->courses = list_init(course_clone, course_destroy);
	if (new_stu->courses == NULL) {
			free(new_stu->name);
			free(new_stu);
			return NULL;
		}

	return new_stu;
}

/**
 * @brief Initializes the "grades" data-structure.
 * @returns A pointer to the data-structure, of NULL in case of an error
 */
struct grades* grades_init() {
	struct grades *grades = malloc(sizeof(*grades));
	if (!grades) {
		return NULL;
	}

	grades->students = list_init(student_clone, student_destroy);
	if (!grades->students) {
		free(grades);
		return NULL;
	}

	return grades;
}

/**
 * @brief Destroys "grades", de-allocate all memory!
 */
void grades_destroy(struct grades *grades) {
	if (grades != NULL) {
		list_destroy(grades->students);
		free(grades);
	}
}

/**
 * @brief Adds a student with "name" and "id" to "grades"
 * @returns 0 on success
 * @note Failes if "grades" is invalid, or a student with
 * the same "id" already exists in "grades"
 */
int grades_add_student(struct grades *grades, const char *name, int id) {
	if (!grades || grades_search(grades->students, id) != NULL) {
		return 1;
	}

	Student new_student = student_create(name, id);
	if (!new_student) {
		return 1;
	}

	if (list_push_back(grades->students, new_student) != 0) {
		student_destroy(new_student);
		return 1;
	}

	/** de-allocate memory of new_student as list_push_back creates a clone **/
	student_destroy(new_student);
	return 0;
}

/**
 * @brief Adds a course with "name" and "grade" to the student with "id"
 * @return 0 on success
 * @note Failes if "grades" is invalid, if a student with "id" does not exist
 * in "grades", if the student already has a course with "name", or if "grade"
 * is not between 0 to 100.
 */
int grades_add_grade(struct grades *grades, const char *name, int id, int grade) {
	if (!grades || !name || grade < 0 || grade > 100) {
		return 1;
  	  }

   	struct iterator *it_student = grades_search(grades->students, id);
	if (it_student == NULL) {
        	return 1; // student not found
	}
	// Check if course with same name already exists
   	for (struct iterator *it_course = list_begin(stu->courses);
         	it_course != NULL;
        	it_course = list_next(it_course)) {
       		Course c = (Course) list_get(it_course);
        	if (strcmp(c->name, name) == 0) {
           		return 1; // course already exists
        	}
    	}
	// Create new course
	Course new_course = course_create(name, grade);
	if (new_course == NULL) {
        	return 1; // failed to create
	}

	// Add course to the courses list
    	if (list_push_back(stu->courses, new_course) != 0) {
        	course_destroy(new_course); // cleanup on failure
        	return 1;
    	}

    	// list_push_back creates a clone, so we can free our original
   	course_destroy(new_course);
    	return 0;
}

/**
 * @brief Calcs the average of the student with "id" in "grades".
 * @param[out] out This method sets the variable pointed by "out" to the
 * student's name. Needs to allocate memory. The user is responsible for
 * freeing the memory.
 * @returns The average, or -1 on error
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note If the student has no courses, the average is 0.
 * @note On error, sets "out" to NULL.
 */
float grades_calc_avg(struct grades *grades, int id, char **out){
	if(!grades)
		return -1;
	struct iterator *it_student = grades_search(grades->students, id);
	if (it_student == NULL) {
        	return 1; // student not found
	}
	int counter = 0, sum = 0;
	float avg = 0;
	for (struct iterator *it_course = list_begin(stu->courses);
         	it_course != NULL;
        	it_course = list_next(it_course)) {
		Course c = (Course) list_get(it_course);
		counter++;
		sum += c->grade;
	}
	if(counter == 0) {
		return 0;
	}
	avg = sum/counter;
	return avg;
}

/**
 * @brief Prints the courses of the student with "id" in the following format:
 * STUDENT-NAME STUDENT-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid, or if a student with "id" does not exist
 * in "grades".
 * @note The courses should be printed according to the order 
 * in which they were inserted into "grades"
 */
int grades_print_student(struct grades *grades, int id);

/**
 * @brief Prints all students in "grade", in the following format:
 * STUDENT-1-NAME STUDENT-1-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * STUDENT-2-NAME STUDENT-2-ID: COURSE-1-NAME COURSE-1-GRADE, [...]
 * @returns 0 on success
 * @note Fails if "grades" is invalid
 * @note The students should be printed according to the order 
 * in which they were inserted into "grades"
 * @note The courses should be printed according to the order 
 * in which they were inserted into "grades"
 */
int grades_print_all(struct grades *grades);











