#include <stdio.h>
#include <csptr/smart_ptr.h>
#include <datatype.h>

typedef struct
{
    int age;
    char name[50];
} student;

student *new_student(int a, char *n)
{
    smart student *s = unique_ptr(student, {.name = "altair", .age = a});
    return s;
}

int main()
{
    printf("std: %d \n", __STDC__);
    printf("std: %ld \n", __STDC_VERSION__);
    printf("file: %s\n", __FILE__);
    printf("line: %d\n", __LINE__);
    printf("pointer siz e-> %d\n", sizeof(void *));
    printf("long long size -> %d\n", sizeof(long long));
    if (T)
    {
        student *s = new_student(10, "altair");
        printf("%d %s\n", s->age, s->name);
    }
}