# Bike Database Management System

![C++](https://img.shields.io/badge/C++-17-blue)
![ODBC](https://img.shields.io/badge/ODBC-3.8-green)
![SQL Server](https://img.shields.io/badge/SQL_Server-2019-orange)

A console-based application for managing bike-related user accounts with SQL Server database integration.

## Features

- **User Account Management**
  - Create new user accounts with email, username, and password
  - Secure login with password masking
  - Update account information (username, email, or password)
  - Input validation and duplicate checking

- **Database Operations**
  - ODBC connection to SQL Server
  - SQL query execution with error handling
  - Real-time data validation
  - Secure password handling

- **User Interface**
  - Console-based menu system
  - Clear screen transitions
  - Keyboard navigation (Esc to exit/cancel)
  - Immediate feedback for operations

## Technologies Used

- C++17
- ODBC API (SQL.h, sqlext.h)
- SQL Server Express
- Windows API (for console operations)

## Prerequisites

- SQL Server Express (local instance)
- ODBC Driver for SQL Server
- C++ compiler with C++17 support
- Windows operating system

## Installation

1. **Set up the database**
   - Create a database named `bikedatabase` on your SQL Server instance
   - Create a table named `klient` with columns:
     - `Email` (nvarchar)
     - `Nick` (nvarchar)
     - `Haslo` (nvarchar)

2. **Configure ODBC**
   - Ensure you have the ODBC Driver for SQL Server installed
   - Verify the connection string matches your server name:
     ```cpp
     L"DRIVER={SQL Server};SERVER=DESKTOP-I61N0IU\\SQLEXPRESS;DATABASE=bikedatabase;Trusted_Connection=yes;"
     ```

3. **Compile the application**
   ```bash
   g++ main.cpp -o BikeManagementSystem
