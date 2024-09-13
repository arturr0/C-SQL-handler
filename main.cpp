#include <iostream>
#include <conio.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>


void PerformInsert(SQLHDBC hdbc);
void PerformSelect(SQLHDBC hdbc, bool isLoginAfterInsert);
void PerformUpdate(SQLHDBC hdbc);
void ShowReturnOptions();

int main() {
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

    SQLWCHAR conn_str[] = L"DRIVER={SQL Server};SERVER=DESKTOP-I61N0IU\\SQLEXPRESS;DATABASE=bikedatabase;Trusted_Connection=yes;";

    retcode = SQLDriverConnect(hdbc, NULL, conn_str, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        std::wcout << L"Connected to the database successfully!" << std::endl;

        while (true) {
            std::wcout << L"Select operation:" << std::endl;
            std::wcout << L"1. Create account" << std::endl;
            std::wcout << L"2. Log in" << std::endl;
            std::wcout << L"3. Update" << std::endl;
            std::wcout << L"Press Esc to exit" << std::endl;
            std::wcout << L"Enter your choice (1, 2, 3, or Esc): ";

            int choice = _getch();

            if (choice == 27) {
                system("cls");
                break;
            }

            if (choice == '1') {
                system("cls");
                PerformInsert(hdbc);
                //ShowReturnOptions();
            }
            else if (choice == '2') {
                system("cls");
                PerformSelect(hdbc, false);
                //ShowReturnOptions();
            }
            else if (choice == '3') {
                system("cls");
                PerformUpdate(hdbc);
                //ShowReturnOptions();
            }
            else {
                system("cls");
                std::wcerr << L"Invalid choice. Try again." << std::endl;
            }
        }
    }
    else {
        std::wcerr << L"Failed to connect to the database!" << std::endl;
    }

    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

    return 0;
}

void PerformInsert(SQLHDBC hdbc) {
    SQLHSTMT hstmt_insert;
    SQLHSTMT hstmt_select;
    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_insert);
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_select);

    std::wcout << L"Enter your data" << std::endl;

    SQLWCHAR insertQuery[512];
    SQLWCHAR selectQuery[512];
    std::wstring newEmail, newUsername, newPassword;

    std::wcout << L"Enter your email: ";
    std::getline(std::wcin, newEmail);
    std::wcout << L"Enter your username: ";
    std::getline(std::wcin, newUsername);

    _snwprintf_s(selectQuery, 511, L"SELECT * FROM klient WHERE Email = '%s' OR Nick = '%s'", newEmail.c_str(), newUsername.c_str());
    retcode = SQLExecDirect(hstmt_select, selectQuery, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hstmt_select) == SQL_SUCCESS) {
            system("cls");
            std::wcout << L"Cannot update. Email or username already exists." << std::endl;
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_insert);
            return;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);

    std::wcout << L"Enter your password: ";
    std::getline(std::wcin, newPassword);

    _snwprintf_s(insertQuery, 511, L"INSERT INTO klient (Email, Nick, Haslo) VALUES ('%s', '%s', '%s')",
        newEmail.c_str(), newUsername.c_str(), newPassword.c_str());

    retcode = SQLExecDirect(hstmt_insert, insertQuery, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        system("cls");
        std::wcout << L"New account created successfully!" << std::endl;
        ShowReturnOptions();
    }
    else {
        SQLSMALLINT recNumber = 1;
        SQLWCHAR sqlState[6];
        SQLINTEGER nativeError;
        SQLWCHAR messageText[256];
        SQLSMALLINT textLength;

        while (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt_insert, recNumber, sqlState, &nativeError, messageText, sizeof(messageText), &textLength) == SQL_SUCCESS) {
            std::wcerr << L"SQL Error (Insert): " << sqlState << L" - " << nativeError << L" - " << messageText << std::endl;
            recNumber++;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_insert);
}

void PerformSelect(SQLHDBC hdbc, bool isLoginAfterInsert) {
    SQLHSTMT hstmt_select;
    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_select);

    if (!isLoginAfterInsert) {
        std::wcout << L"Log in operation selected!" << std::endl;
    }

    SQLWCHAR query_select[512];
    std::wstring email, username, password;

    std::wcout << L"Enter email: ";
    std::getline(std::wcin, email);
    std::wcout << L"Enter username: ";
    std::getline(std::wcin, username);
    std::wcout << L"Enter password: ";

    // Masking the password input
    wchar_t ch;
    while ((ch = _getch()) != 13) { // 13 is Enter key
        if (ch == 8) { // 8 is Backspace
            if (!password.empty()) {
                password.pop_back();
                std::wcout << L"\b \b"; // Erase the last asterisk
            }
        }
        else {
            password.push_back(ch);
            std::wcout << L'*';
        }
    }
    std::wcout << std::endl;

    _snwprintf_s(query_select, 511, L"SELECT * FROM klient WHERE Email = '%s' AND Nick = '%s' AND Haslo = '%s'",
        email.c_str(), username.c_str(), password.c_str());

    retcode = SQLExecDirect(hstmt_select, query_select, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hstmt_select) == SQL_SUCCESS) {
            system("cls");
            std::wcout << L"Logged in!" << std::endl;
            ShowReturnOptions();
        }
        else {
            system("cls");
            std::wcout << L"You entered wrong data." << std::endl;
        }
    }
    else {
        SQLSMALLINT recNumber = 1;
        SQLWCHAR sqlState[6];
        SQLINTEGER nativeError;
        SQLWCHAR messageText[256];
        SQLSMALLINT textLength;

        while (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt_select, recNumber, sqlState, &nativeError, messageText, sizeof(messageText), &textLength) == SQL_SUCCESS) {
            std::wcerr << L"SQL Error (Select): " << sqlState << L" - " << nativeError << L" - " << messageText << std::endl;
            recNumber++;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
}
void PerformUpdate(SQLHDBC hdbc) {
    SQLHSTMT hstmt_select;
    SQLHSTMT hstmt_update;
    SQLRETURN retcode;

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_select);
    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt_update);

    std::wcout << L"Log in to update" << std::endl;

    SQLWCHAR selectQuery[512];
    SQLWCHAR updateQuery[512];
    std::wstring email, username, password, newNick, newEmail, newPassword;
    int updateChoice;

    std::wcout << L"Enter email: ";
    while (true) {
        int key = _getch();
        if (key == 27) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
            system("cls");
            return;
        }
        else if (key == 13) {

            break;
        }
        else {
            email.push_back(static_cast<wchar_t>(key));
            std::wcout << static_cast<wchar_t>(key);
        }
    }

    std::wcout << L"\nEnter username: ";
    while (true) {
        int key = _getch();
        if (key == 27) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
            system("cls");
            return;
        }
        else if (key == 13) {
            break;
        }
        else {
            username.push_back(static_cast<wchar_t>(key));
            std::wcout << static_cast<wchar_t>(key);
        }
    }

    std::wcout << L"\nEnter password: ";
    while (true) {
        int key = _getch();
        if (key == 27) {
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
            system("cls");
            return;
        }
        else if (key == 13) {
            break;
        }
        else {
            password.push_back(static_cast<wchar_t>(key));
            std::wcout << L'*';
        }
    }

    _snwprintf_s(selectQuery, 511, L"SELECT * FROM klient WHERE Email = '%s' AND Nick = '%s' AND Haslo = '%s'",
        email.c_str(), username.c_str(), password.c_str());

    retcode = SQLExecDirect(hstmt_select, selectQuery, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hstmt_select) != SQL_SUCCESS) {
            system("cls");
            std::wcout << L"You entered wrong data. Update canceled." << std::endl;
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
            return;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
    system("cls");
    std::wcout << L"Choose what to update:" << std::endl;
    std::wcout << L"1. Update username" << std::endl;
    std::wcout << L"2. Update email" << std::endl;
    std::wcout << L"3. Update password" << std::endl;
    std::wcout << L"Enter your choice (1, 2, 3, or Esc): ";

    updateChoice = _getch();

    while (_kbhit()) {
        _getch();
    }

    if (updateChoice == 27) {
        system("cls");
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
        return;
    }

    std::wcout << static_cast<wchar_t>(updateChoice) << std::endl;

    switch (updateChoice) {
    case '1':
        system("cls");
        std::wcout << L"\Enter new username: ";
        while (true) {
            int key = _getch();
            if (key == 27) {
                SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
                system("cls");
                return;
            }
            else if (key == 13) {
                break;
            }
            else {
                newNick.push_back(static_cast<wchar_t>(key));
                std::wcout << static_cast<wchar_t>(key);
            }
        }

        _snwprintf_s(updateQuery, 511, L"UPDATE klient SET Nick = '%s' WHERE Email = '%s' AND Nick = '%s' AND Haslo = '%s'",
            newNick.c_str(), email.c_str(), username.c_str(), password.c_str());
        break;

    case '2':
        system("cls");
        std::wcout << L"\nEnter new email: ";
        while (true) {
            int key = _getch();
            if (key == 27) {
                SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
                system("cls");
                return;
            }
            else if (key == 13) {
                break;
            }
            else {
                newEmail.push_back(static_cast<wchar_t>(key));
                std::wcout << static_cast<wchar_t>(key);
            }
        }

        // Check if the new Email already exists
        _snwprintf_s(selectQuery, 511, L"SELECT * FROM klient WHERE Email = '%s'", newEmail.c_str());
        retcode = SQLExecDirect(hstmt_select, selectQuery, SQL_NTS);

        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            if (SQLFetch(hstmt_select) == SQL_SUCCESS) {
                system("cls");
                std::wcout << L"Cannot update. Email already exists." << std::endl;
                SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
                return;
            }
        }

        _snwprintf_s(updateQuery, 511, L"UPDATE klient SET Email = '%s' WHERE Email = '%s' AND Nick = '%s' AND Haslo = '%s'",
            newEmail.c_str(), email.c_str(), username.c_str(), password.c_str());
        break;

    case '3':
        system("cls");
        std::wcout << L"\Enter new password: ";
        while (true) {
            int key = _getch();
            if (key == 27) {
                SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
                system("cls");
                return;
            }
            else if (key == 13) {
                break;
            }
            else {
                newPassword.push_back(static_cast<wchar_t>(key));
                std::wcout << L'*';
            }
        }

        _snwprintf_s(updateQuery, 511, L"UPDATE klient SET Haslo = '%s' WHERE Email = '%s' AND Nick = '%s' AND Haslo = '%s'",
            newPassword.c_str(), email.c_str(), username.c_str(), password.c_str());
        break;

    default:
        system("cls");
        std::wcerr << L"Invalid choice. Update canceled." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
        return;
    }

    // Check if the new Nick already exists
    _snwprintf_s(selectQuery, 511, L"SELECT * FROM klient WHERE Nick = '%s'", newNick.c_str());
    retcode = SQLExecDirect(hstmt_select, selectQuery, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        if (SQLFetch(hstmt_select) == SQL_SUCCESS) {
            system("cls");
            std::wcout << L"Cannot update. Username already exists." << std::endl;
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
            SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
            return;
        }
    }

    // Execute the update query
    retcode = SQLExecDirect(hstmt_update, updateQuery, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        system("cls");
        std::wcout << L"Update successful!" << std::endl;
        ShowReturnOptions();
    }
    else {
        SQLSMALLINT recNumber = 1;
        SQLWCHAR sqlState[6];
        SQLINTEGER nativeError;
        SQLWCHAR messageText[256];
        SQLSMALLINT textLength;

        while (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt_update, recNumber, sqlState, &nativeError, messageText, sizeof(messageText), &textLength) == SQL_SUCCESS) {
            //std::wcerr << L"SQL Error (Update): " << sqlState << L" - " << nativeError << L" - " << messageText << std::endl;
            system("cls");
            std::wcout << L"Cannot update. Data already exists." << std::endl;
            recNumber++;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_update);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_select);
}




void ShowReturnOptions() {
    while (true) {
        std::wcout << L"APP" << std::endl;
        std::wcout << L"1. Check distance" << std::endl;
        std::wcout << L"2. Check parts" << std::endl;
        std::wcout << L"Press Esc to exit" << std::endl;
        std::wcout << L"Enter your choice (1, 2, or Esc): ";

        int optionsChoice = _getch();



        if (optionsChoice == 27) {
            system("cls");
            // Handle Esc key press
            // For now, you can choose to do nothing or exit the program
            break;
        }
        else if (optionsChoice == '1') {
            system("cls");
            // Return to Select operation
            // You can add the necessary code here to perform the return action
            std::wcout << L"Your distance is..." << std::endl;
        }
        else if (optionsChoice == '2') {
            system("cls");
            // Exit the program
            std::wcout << L"Your parts..." << std::endl;
            //exit(0);
        }
        else {
            system("cls");
            std::cout << L"Invalid choice. Try again." << std::endl;
            // You can choose to handle the invalid choice in a specific way
            // For now, I just clear the screen and display an error message
        }
    }
}
