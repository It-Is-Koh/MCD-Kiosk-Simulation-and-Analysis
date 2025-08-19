# MCD-Kiosk-Simulation-and-Analysis<br>(McDonald's Kiosk Ordering & Analysis System)

This project simulates a **McDonald's-style self-ordering kiosk** with two main systems and one utility tool:

1. **Ordering System (`MCD_Ordering_System`)** – allows customers to place, confirm, and cancel orders. It generates receipts and daily statistics files.  
2. **Analyzing System (`MCD_Analyzing_System`)** – processes daily statistics, calculates revenue, and ranks sold items.  
3. **Testing Tool (`randomlyGenerateStatisticsFile`)** – generates random statistics for testing the analyzing system.

---

## Features

### Ordering System (`MCD_Ordering_System.cpp`)

- Orders are made by entering `(Category, Term, Quantity)`  
  - Example: `(1,2,3)` → 2nd item in Category 1, quantity = 3.
- **Special commands**:
  - `(0,1,0)` → Confirm order
  - `(0,2,0)` → Cancel and restart
  - `(0,3,0)` → Display full menu
  - `(0,0,0)` → Close business for the day
- After confirming an order:
  - Displays **subtotal, 6% tax, rounding, total price**
  - Requires payment input and calculates change
- Generates **receipt files** under `data/receipts/<YYYYMMDD>/`  
  - Example: `<YYYYMMDD>_Receipt_No._<N>.txt`
  - Receipt content:
    1. **Header** – McDonald's title, Receipt No., date, and time
    2. **Order details** – Columns for Line No., Category ID, Term ID, Quantity, Price, and Item name, aligned using tabs (`\t`)
    3. **Summary** – Subtotal, 6% tax, rounding adjustment, total amount, payment received, and change
- When **closing the day** `(0,0,0)`:
  - Final receipt indicates that today's operation has ended
  - Shows:
    - Total number of orders for the day
    - Total earnings of the day (including tax)
  - No detailed order list or payment summary is included
- Generates **daily statistics file**:
  - `data/statistics/<YYYYMMDD>_Statistics.txt`
  - Records orders line by line as `Category Term Quantity` in chronological order

### Analyzing System (`MCD_Analyzing_System.cpp`)

- Analyzes `data/statistics/<YYYYMMDD>_Statistics.txt`
- Outputs **daily revenue report** with:
  - Tax-inclusive and tax-exclusive income
  - Tax amount and subtotal
  - Rounding adjustments
- Displays a **tabular report** using Markdown style table:

| Category | Term | Unit Price | Quantity | Rank/Q | Daily Earns | Rank/DE | Item Name |
|----------|------|------------|----------|--------|-------------|---------|-----------|
| 1        | 2    | 50         | 4        | 2      | 200         | 3       | Cheeseburger |

- Ranking views:
  1. **By Quantity sold** – only sold items shown
  2. **By Daily Earnings** – only sold items shown
- Each section is also saved into separate files under `data/statistics/`:
  - `<YYYYMMDD>_Overall_Results.txt`
  - `<YYYYMMDD>_Rank_With_Quantity.txt`
  - `<YYYYMMDD>_Rank_With_Daily_Earns.txt`

### Testing Tool (`randomlyGenerateStatisticsFile.cpp`)

- Generates random statistics files in `data/test_statistics/`
- Developers can copy these test files into `data/statistics/` for testing the Analyzing System

---

## Project Structure

This project is organized into four main directories:
- `MCD_Ordering_System/` - Contains the source code and build files for the main ordering application.
- `MCD_Analyzing_System/` - Contains the source code and build files for the data analysis tool.
- `tools/` - Includes a helper tool for generating test statistics.
- `data/` - Holds all data files, including menus, receipts, and statistics.

---

## How to Run

### 1. Ordering System

Change directory and run the executable:

    cd MCD_Ordering_System
    make
    ./MCD_Ordering_System.exe

- Place orders using `(Category, Term, Quantity)` format
- Receipts will be saved in `data/receipts/<YYYYMMDD>/<YYYYMMDD>_Receipt_No._<N>.txt`
- Daily statistics saved in `data/statistics/<YYYYMMDD>_Statistics.txt`

---

### 2. Analyzing System

Change directory and run the executable:

    cd MCD_Analyzing_System
    make
    ./MCD_Analyzing_System.exe

- Reads `data/statistics/<YYYYMMDD>_Statistics.txt`
- Generates three analysis files:
  - `<YYYYMMDD>_Overall_Results.txt`
  - `<YYYYMMDD>_Rank_With_Quantity.txt`
  - `<YYYYMMDD>_Rank_With_Daily_Earns.txt`

---

### 3. Testing Tool

Change directory and run the executable:

    cd tools/randomlyGenerateStatisticsFile
    make
    ./randomlyGenerateStatisticsFile.exe

- Creates `<YYYYMMDD>_Statistics.txt` in `data/test_statistics/`
- Move or copy the file into `data/statistics/` to test the Analyzing System

---

## Example Workflow

1. Run **Ordering System** → Take orders, generate receipts & statistics file
2. Run **Analyzing System** → Analyze the generated statistics
3. (Optional) Use **Testing Tool** to simulate order data

---

## Notes

- File naming convention follows date-based format: `<YYYYMMDD>`
- Receipts are numbered sequentially within the same day
- The final receipt of the day includes business closure details

---

> This project was created as a **final programming course project**, simulating a fast-food ordering & analyzing system.
