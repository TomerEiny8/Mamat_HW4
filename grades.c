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















