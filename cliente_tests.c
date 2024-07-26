#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "claves/claves.h"

int assert_equals_int(int test, int expected, char *message){
    printf("%s\n", message);
    printf("Expected output: %d\n", expected);
    printf("Test output: %d\n", test);
    if (test == expected){
        printf("Test passed\n\n");
    } else {
        printf("Test FAILED\n\n");
    }
}

int assert_equals_str(char *test, char *expected, char *message){
    printf("%s\n", message);
    printf("Expected output: %s\n", expected);
    printf("Test output: %s\n", test);
    if (strcmp(test, expected) == 0){
        printf("Test passed\n\n");
    } else {
        printf("Test FAILED\n\n");
    }
}

int assert_equals_double(double test, double expected, char *message){
    printf("%s\n", message);
    printf("Expected output: %f\n", expected);
    printf("Test output: %f\n", test);
    if (test == expected){
        printf("Test passed\n\n");
    } else {
        printf("Test FAILED\n\n");
    }
}

int main(int argc, char *argv[])
{
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];

    char client_name[256];
    sprintf(client_name, "client_%d", getpid());
    // Test all the functions in claves.h
    printf("-------- TESTING ALL THE FUNCTIONS: %s --------\n", client_name);

    key = 1;
    strcpy(value1, "value1");
    N_value2 = 3;
    for (int i = 0; i < N_value2; i++){
        V_value2[i] = i + 1.0;
    }


    // Test all the functions in claves.h before init() has been run
    printf("-------- TESTING ALL THE FUNCTIONS BEFORE INIT() --------\n");

    // Delete the file "tuplas.txt"
    remove("tuplas.txt");
    
    // Test set_value
    int test_set_value_error_1 = set_value(key, value1, N_value2, V_value2);
    int expected_set_value_error_1 = -1;
    assert_equals_int(test_set_value_error_1, expected_set_value_error_1, "Test set_value(1, \"value1\", 3, [1.0, 2.0, 3.0])");

    printf("value1: %s\n", value1);

    // Test get_value
    int test_get_value_error_1 = get_value(key, value1, &N_value2, V_value2);
    int expected_get_value_error_1 = -1;
    assert_equals_int(test_get_value_error_1, expected_get_value_error_1, "Test get_value(1, value1, &N_value2, V_value2)");

    // Now value1 is NULL, N_value2 is 0 and V_value2 is NULL (since we did not get anything with get_value()), so we have to set them again
    strcpy(value1, "value1");
    N_value2 = 3;
    for (int i = 0; i < N_value2; i++){
        V_value2[i] = i + 1.0;
    }

    // Test modify_value
    int test_modify_value_error_1 = modify_value(key, value1, N_value2, V_value2);
    int expected_modify_value_error_1 = -1;
    assert_equals_int(test_modify_value_error_1, expected_modify_value_error_1, "Test modify_value(1, \"value1\", 3, [1.0, 2.0, 3.0])");

    // Test exist
    int test_exist_error_1 = exist(key);
    int expected_exist_error_1 = -1;
    assert_equals_int(test_exist_error_1, expected_exist_error_1, "Test exist()");

    // Test delete_key
    int test_delete_key_error_1 = delete_key(key);
    int expected_delete_key_error_1 = -1;
    assert_equals_int(test_delete_key_error_1, expected_delete_key_error_1, "Test delete_key()");



    // Test all the functions in claves.h after init() has been run
    printf("-------- TESTING ALL THE FUNCTIONS AFTER INIT() --------\n");

    // Test init
    int test_init = init();
    int expected_init = 0;
    assert_equals_int(test_init, expected_init, "Test init()");

    // Test set_value
    int test_set_value_1 = set_value(key, value1, N_value2, V_value2);
    int expected_set_value_1 = 0;
    assert_equals_int(test_set_value_1, expected_set_value_1, "Test set_value(1, \"value1\", 3, [1.0, 2.0, 3.0])");
    
    char value1_get[256];
    int N_value2_get;
    double V_value2_get[32];


    // Test get_value
    int test_get_value_1 = get_value(key, value1_get, &N_value2_get, V_value2_get);
    int expected_get_value_1 = 0;
    assert_equals_int(test_get_value_1, expected_get_value_1, "Test get_value(1, value1, &N_value2, V_value2)");

    // Check that the values set with set_value are the same as the ones returned by get_value
    assert_equals_int(key, 1, "Check that key has not been modified");
    assert_equals_str(value1_get, "value1", "Check that value1 has been modified");
    assert_equals_int(N_value2_get, 3, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_get; i++){
        assert_equals_double(V_value2_get[i], i + 1.0, "Check that V_value2[i] has been modified");
    }


    // New values to modify
    strcpy(value1, "value1_modified");
    N_value2 = 5;
    for (int i = 0; i < N_value2; i++){
        V_value2[i] = 2*i + 2.0;
    }

    // Test modify_value
    int test_modify_value_1 = modify_value(key, value1, N_value2, V_value2);
    int expected_modify_value_1 = 0;
    assert_equals_int(test_modify_value_1, expected_modify_value_1, "Test modify_value(1, \"value1_modified\", 5, [2.0, 4.0, 6.0, 8.0, 10.0])");

    // Check that the values set with modify_value are the same as the ones returned by get_value
    int test_get_value_2 = get_value(key, value1_get, &N_value2_get, V_value2_get);
    int expected_get_value_2 = 0;
    assert_equals_int(test_get_value_2, expected_get_value_2, "Test get_value(1, value1, &N_value2, V_value2)");

    assert_equals_int(key, 1, "Check that key has not been modified");
    assert_equals_str(value1_get, "value1_modified", "Check that value1 has been modified");
    assert_equals_int(N_value2_get, 5, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_get; i++){
        assert_equals_double(V_value2_get[i], 2*i + 2.0, "Check that V_value2[i] has been modified");
    }


    // Test exist (it should return 1)
    int test_exist_1 = exist(key);
    int expected_exist_1 = 1;
    assert_equals_int(test_exist_1, expected_exist_1, "Test exist()");

    
    // Test delete_key
    int test_delete_key_1 = delete_key(key);
    int expected_delete_key_1 = 0;
    assert_equals_int(test_delete_key_1, expected_delete_key_1, "Test delete_key()");
    

    // Test exist (now it should return 0)
    int test_exist_2 = exist(key);
    int expected_exist_2 = 0;
    assert_equals_int(test_exist_2, expected_exist_2, "Test exist()");



    // Insert a value and then do init to check that it is deleted
    printf("-------- TESTING THAT INIT DELETES ALL THE TUPLES --------\n");
    int test_set_value_2 = set_value(key, value1, N_value2, V_value2);
    int expected_set_value_2 = 0;
    assert_equals_int(test_set_value_2, expected_set_value_2, "Do set_value(1, \"value1_modified\", 5, [2.0, 4.0, 6.0, 8.0, 10.0])");

    int test_init_2 = init();
    int expected_init_2 = 0;
    assert_equals_int(test_init_2, expected_init_2, "Delete the file content with init()");

    // Check that the value has been deleted
    int test_exist_3 = exist(key);
    int expected_exist_3 = 0;
    assert_equals_int(test_exist_3, expected_exist_3, "Test that the inserted value does not exist with exist()");

    printf("-------- TESTING SEVERAL TUPLES AT THE SAME TIME --------\n");
    // Insert several tuples at the same time
    int test_set_value_3 = set_value(1, "value1", 3, (double[]){1.0, 2.0, 3.0});
    int expected_set_value_3 = 0;
    assert_equals_int(test_set_value_3, expected_set_value_3, "Do set_value(1, \"value1\", 3, [1.0, 2.0, 3.0])");

    int test_set_value_4 = set_value(2, "value2", 4, (double[]){4.0, 5.0, 6.0, 7.0});
    int expected_set_value_4 = 0;
    assert_equals_int(test_set_value_4, expected_set_value_4, "Do set_value(2, \"value2\", 4, [4.0, 5.0, 6.0, 7.0])");

    int test_set_value_5 = set_value(3, "value3", 5, (double[]){8.0, 9.0, 10.0, 11.0, 12.0});
    int expected_set_value_5 = 0;
    assert_equals_int(test_set_value_5, expected_set_value_5, "Do set_value(3, \"value3\", 5, [8.0, 9.0, 10.0, 11.0, 12.0])");

    // Check that the values have been inserted
    int test_exist_4 = exist(1);
    int expected_exist_4 = 1;
    assert_equals_int(test_exist_4, expected_exist_4, "Test that the inserted value exists with exist()");
    int test_exist_5 = exist(2);
    int expected_exist_5 = 1;
    assert_equals_int(test_exist_5, expected_exist_5, "Test that the inserted value exists with exist()");
    int test_exist_6 = exist(3);
    int expected_exist_6 = 1;
    assert_equals_int(test_exist_6, expected_exist_6, "Test that the inserted value exists with exist()");

    // Get the values
    char value1_1[256];
    int N_value2_1;
    double V_value2_1[32];
    int test_get_value_3 = get_value(1, value1_1, &N_value2_1, V_value2_1);
    int expected_get_value_3 = 0;
    assert_equals_int(test_get_value_3, expected_get_value_3, "Test get_value(1, value1, &N_value2, V_value2)");
    assert_equals_str(value1_1, "value1", "Check that value1 has been modified");
    assert_equals_int(N_value2_1, 3, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_1; i++){
        assert_equals_double(V_value2_1[i], i + 1.0, "Check that V_value2[i] has been modified");
    }

    char value1_2[256];
    int N_value2_2;
    double V_value2_2[32];
    int test_get_value_4 = get_value(2, value1_2, &N_value2_2, V_value2_2);
    int expected_get_value_4 = 0;
    assert_equals_int(test_get_value_4, expected_get_value_4, "Test get_value(2, value1, &N_value2, V_value2)");
    assert_equals_str(value1_2, "value2", "Check that value1 has been modified");
    assert_equals_int(N_value2_2, 4, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_2; i++){
        assert_equals_double(V_value2_2[i], i + 4.0, "Check that V_value2[i] has been modified");
    }

    char value1_3[256];
    int N_value2_3;
    double V_value2_3[32];
    int test_get_value_5 = get_value(3, value1_3, &N_value2_3, V_value2_3);
    int expected_get_value_5 = 0;
    assert_equals_int(test_get_value_5, expected_get_value_5, "Test get_value(3, value1, &N_value2, V_value2)");
    assert_equals_str(value1_3, "value3", "Check that value1 has been modified");
    assert_equals_int(N_value2_3, 5, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_3; i++){
        assert_equals_double(V_value2_3[i], i + 8.0, "Check that V_value2[i] has been modified");
    }

    // Modify the values
    strcpy(value1_1, "value1_modified");
    N_value2_1 = 5;
    for (int i = 0; i < N_value2_1; i++){
        V_value2_1[i] = 2*i + 2.0;
    }

    strcpy(value1_2, "value2_modified");
    N_value2_2 = 6;
    for (int i = 0; i < N_value2_2; i++){
        V_value2_2[i] = 2*i + 3.0;
    }

    strcpy(value1_3, "value3_modified");
    N_value2_3 = 7;
    for (int i = 0; i < N_value2_3; i++){
        V_value2_3[i] = 2*i + 4.0;
    }

    int test_modify_value_2 = modify_value(1, value1_1, N_value2_1, V_value2_1);
    int expected_modify_value_2 = 0;
    assert_equals_int(test_modify_value_2, expected_modify_value_2, "Test modify_value(1, \"value1_modified\", 5, [2.0, 4.0, 6.0, 8.0, 10.0])");

    int test_modify_value_3 = modify_value(2, value1_2, N_value2_2, V_value2_2);
    int expected_modify_value_3 = 0;
    assert_equals_int(test_modify_value_3, expected_modify_value_3, "Test modify_value(2, \"value2_modified\", 6, [3.0, 5.0, 7.0, 9.0, 11.0, 13.0])");

    int test_modify_value_4 = modify_value(3, value1_3, N_value2_3, V_value2_3);
    int expected_modify_value_4 = 0;
    assert_equals_int(test_modify_value_4, expected_modify_value_4, "Test modify_value(3, \"value3_modified\", 7, [4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0])");

    // Check that the values set with modify_value are the same as the ones returned by get_value
    char value1_1_get[256];
    int N_value2_1_get;
    double V_value2_1_get[32];
    int test_get_value_6 = get_value(1, value1_1_get, &N_value2_1_get, V_value2_1_get);
    int expected_get_value_6 = 0;
    assert_equals_int(test_get_value_6, expected_get_value_6, "Test get_value(1, value1, &N_value2, V_value2)");

    assert_equals_str(value1_1_get, "value1_modified", "Check that value1 has been modified");
    assert_equals_int(N_value2_1_get, 5, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_1_get; i++){
        assert_equals_double(V_value2_1_get[i], 2*i + 2.0, "Check that V_value2[i] has been modified");
    }

    char value1_2_get[256];
    int N_value2_2_get;
    double V_value2_2_get[32];
    int test_get_value_7 = get_value(2, value1_2_get, &N_value2_2_get, V_value2_2_get);
    int expected_get_value_7 = 0;
    assert_equals_int(test_get_value_7, expected_get_value_7, "Test get_value(2, value1, &N_value2, V_value2)");

    assert_equals_str(value1_2_get, "value2_modified", "Check that value1 has been modified");
    assert_equals_int(N_value2_2_get, 6, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_2_get; i++){
        assert_equals_double(V_value2_2_get[i], 2*i + 3.0, "Check that V_value2[i] has been modified");
    }

    char value1_3_get[256];
    int N_value2_3_get;
    double V_value2_3_get[32];
    int test_get_value_8 = get_value(3, value1_3_get, &N_value2_3_get, V_value2_3_get);
    int expected_get_value_8 = 0;
    assert_equals_int(test_get_value_8, expected_get_value_8, "Test get_value(3, value1, &N_value2, V_value2)");

    assert_equals_str(value1_3_get, "value3_modified", "Check that value1 has been modified");
    assert_equals_int(N_value2_3_get, 7, "Check that N_value2 has been modified");
    for (int i = 0; i < N_value2_3_get; i++){
        assert_equals_double(V_value2_3_get[i], 2*i + 4.0, "Check that V_value2[i] has been modified");
    }

    // Test delete_key
    int test_delete_key_2 = delete_key(1);
    int expected_delete_key_2 = 0;
    assert_equals_int(test_delete_key_2, expected_delete_key_2, "Test delete_key()");
    int test_delete_key_3 = delete_key(2);
    int expected_delete_key_3 = 0;
    assert_equals_int(test_delete_key_3, expected_delete_key_3, "Test delete_key()");
    int test_delete_key_4 = delete_key(3);
    int expected_delete_key_4 = 0;
    assert_equals_int(test_delete_key_4, expected_delete_key_4, "Test delete_key()");

    // Test exist (now it should return 0)
    int test_exist_7 = exist(1);
    int expected_exist_7 = 0;
    assert_equals_int(test_exist_7, expected_exist_7, "Test exist()");
    int test_exist_8 = exist(2);
    int expected_exist_8 = 0;
    assert_equals_int(test_exist_8, expected_exist_8, "Test exist()");
    int test_exist_9 = exist(3);
    int expected_exist_9 = 0;
    assert_equals_int(test_exist_9, expected_exist_9, "Test exist()");

    return 0;
}