/*
 * ============================================================
 *  Movie Ticket Booking System — v2 (Menu-driven + File I/O)
 *  Fixed layout: 5 rows x 10 columns = 50 seats
 *
 *  Classes
 *  -------
 *  Booking       — holds one confirmed booking's receipt data
 *  Show          — owns the seat grid, handles locking
 *  TheatreSystem — controller: menus, persistence, file I/O
 * ============================================================
 */

#include <iostream>
#include <fstream>        
#include <sstream>        
#include <vector>
#include <unordered_map>
#include <algorithm>      
#include <iomanip>        
#include <limits>         

using namespace std;

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

class Booking {
    string      bookingID;
    string      customerName;
    vector<int> seats;     
    double      amount;

public:
    Booking() : amount(0.0) {}

    Booking(const string&      id,
            const string&      name,
            const vector<int>& seatList,
            double             amt)
        : bookingID(id), customerName(name), seats(seatList), amount(amt) {}

    string         getBookingID()    const { return bookingID;    }
    string         getCustomerName() const { return customerName; }
    const vector<int>& getSeats()   const { return seats;        }
    double         getAmount()       const { return amount;       }

    void printReceipt() const {
        cout << "\n  +====================================+\n";
        cout << "  |        BOOKING RECEIPT             |\n";
        cout << "  +====================================+\n";
        cout << "  | Booking ID   : " << left << setw(20) << bookingID   << "|\n";
        cout << "  | Customer     : " << left << setw(20) << customerName << "|\n";
        cout << "  | Seats booked : ";

        string seatStr;
        for (int s : seats) seatStr += to_string(s) + " ";
        cout << left << setw(20) << seatStr << "|\n";
        cout << "  | Total amount : Rs. " << left << setw(16)
             << (to_string((int)amount) + ".00") << "|\n";
        cout << "\n";
    }

    string serialise() const {
        string line = bookingID + "|" + customerName + "|" + to_string(amount) + "|";
        for (int i = 0; i < (int)seats.size(); i++) {
            if (i) line += ",";
            line += to_string(seats[i]);
        }
        return line;
    }

    static bool deserialise(const string& line, Booking& out) {
        vector<string> parts;
        stringstream   ss(line);
        string         token;
        while (getline(ss, token, '|'))
            parts.push_back(token);

        if (parts.size() != 4) return false;   // malformed

        string      id       = parts[0];
        string      name     = parts[1];
        double      amt      = stod(parts[2]);

        vector<int> seatList;
        stringstream seatStream(parts[3]);
        string seatToken;
        while (getline(seatStream, seatToken, ',')) {
            if (!seatToken.empty())
                seatList.push_back(stoi(seatToken));
        }

        out = Booking(id, name, seatList, amt);
        return true;
    }
};


class Show {
    int          rows;
    int          cols;
    vector<bool> seats;   

public:
    Show(int r = 5, int c = 10) : rows(r), cols(c) {
        seats.assign(r * c, false);
    }

    // ---- Display ----
    void displaySeatMap() const {
        cout << "\n  --------- SEAT MAP ---------\n";
        cout << "  (number = available,  XX = booked)\n\n";

        cout << "       ";
        for (int c = 1; c <= cols; c++)
            cout << setw(5) << c;
        cout << "\n  ";
        cout << string(7 + cols * 5, '-') << "\n";

        for (int r = 0; r < rows; r++) {
            cout << "  Row " << (r + 1) << " |";
            for (int c = 0; c < cols; c++) {
                int seatNo = r * cols + c + 1;
                if (seats[seatNo - 1])
                    cout << setw(5) << "XX";
                else
                    cout << setw(5) << seatNo;
            }
            cout << "\n";
        }
        cout << "  ----------------------------\n";
    }

    bool isValidSeat(int seatNo) const {
        return seatNo >= 1 && seatNo <= rows * cols;
    }

    bool isAvailable(int seatNo) const {
        return isValidSeat(seatNo) && !seats[seatNo - 1];
    }

    // ---- Directly mark a seat as booked (used when loading from file) ----
    void markBooked(int seatNo) {
        if (isValidSeat(seatNo))
            seats[seatNo - 1] = true;
    }

    bool bookSeats(const vector<int>& seatList) {
        for (int s : seatList) {
            if (!isValidSeat(s)) {
                cout << "  Error: Seat " << s
                     << " is out of range (1-" << rows * cols << ").\n";
                return false;
            }
            if (!isAvailable(s)) {
                cout << "  Error: Seat " << s << " is already booked.\n";
                return false;
            }
        }
        for (int s : seatList)
            seats[s - 1] = true;
        return true;
    }

    int    getTotalSeats()    const { return rows * cols; }
    int    getBookedCount()   const { return (int)count(seats.begin(), seats.end(), true); }
    int    getAvailableCount()const { return getTotalSeats() - getBookedCount(); }
    double getOccupancyPct()  const {
        return (double)getBookedCount() / getTotalSeats() * 100.0;
    }
};

class TheatreSystem {
    Show show;
    unordered_map<string, Booking> bookings;
    int bookingCounter;   

    const string FILE_NAME = "bookings.txt";

    // ---- ID generation ----
    string generateBookingID() {
        return "BK" + to_string(++bookingCounter);
    }

    // ------------------------------------------------------------------
    //  File I/O
    // ------------------------------------------------------------------

    void appendToFile(const Booking& b) {
        ofstream ofs(FILE_NAME, ios::app);
        if (!ofs) {
            cout << "  Warning: Could not open " << FILE_NAME << " for writing.\n";
            return;
        }
        ofs << b.serialise() << "\n";
    }

    void loadFromFile() {
        ifstream ifs(FILE_NAME);
        if (!ifs) return;   

        string line;
        int    loadedCount = 0;

        while (getline(ifs, line)) {
            if (line.empty()) continue;

            Booking b;
            if (!Booking::deserialise(line, b)) {
                cout << "  Warning: Skipping malformed line in " << FILE_NAME << ".\n";
                continue;
            }

            // Re-lock seats in the grid
            for (int s : b.getSeats())
                show.markBooked(s);

            string id = b.getBookingID();
            if (id.rfind("BK", 0) == 0) {
                try {
                    int num = stoi(id.substr(2));
                    if (num > bookingCounter) bookingCounter = num;
                } catch (...) {}
            }

            bookings[b.getBookingID()] = b;
            loadedCount++;
        }

        if (loadedCount > 0)
            cout << "  [Info] Loaded " << loadedCount
                 << " booking(s) from " << FILE_NAME << ".\n";
    }

    // ------------------------------------------------------------------
    //  Menu actions
    // ------------------------------------------------------------------

    void menuDisplaySeats() {
        show.displaySeatMap();
    }

    void menuMakeBooking() {
        // --- Get customer name ---
        cout << "\n  Enter customer name: ";
        string customer;
        cin.ignore();                        
        getline(cin, customer);

        if (customer.empty()) {
            cout << "  Error: Customer name cannot be empty.\n";
            return;
        }

        // --- Get seat numbers ---
        cout << "  Enter seat numbers separated by spaces (e.g. 1 2 3): ";
        string seatLine;
        getline(cin, seatLine);

        vector<int> seatList;
        istringstream ss(seatLine);
        int seat;
        while (ss >> seat)
            seatList.push_back(seat);

        if (seatList.empty()) {
            cout << "  Error: No seat numbers entered.\n";
            return;
        }

        // --- Duplicate check within the request ---
        vector<int> sorted = seatList;
        sort(sorted.begin(), sorted.end());
        if (adjacent_find(sorted.begin(), sorted.end()) != sorted.end()) {
            cout << "  Error: Duplicate seat numbers in your request.\n";
            return;
        }

        // --- Try to lock seats ---
        if (!show.bookSeats(seatList)) {
            cout << "  Booking failed. No seats were reserved.\n";
            return;
        }

        // --- Create and store booking ---
        string id     = generateBookingID();
        double amount = (double)seatList.size() * 150.0;   // Rs. 150 per seat
        Booking b(id, customer, seatList, amount);
        bookings[id] = b;

        appendToFile(b);

        cout << "\n  Booking successful!";
        b.printReceipt();
    }

    void menuSearchBooking() {
        cout << "\n  Enter Booking ID to search: ";
        string id;
        cin >> id;

        auto it = bookings.find(id);
        if (it != bookings.end()) {
            cout << "\n  Booking found:";
            it->second.printReceipt();
        } else {
            cout << "  No booking found with ID: " << id << "\n";
        }
    }

    void menuReport() {
        cout << "\n  +=======================================+\n";
        cout << "  |          OCCUPANCY REPORT             |\n";
        cout << "  +=======================================+\n";
        cout << "  | Total seats     : "
             << left << setw(20) << show.getTotalSeats()     << "|\n";
        cout << "  | Booked seats    : "
             << left << setw(20) << show.getBookedCount()    << "|\n";
        cout << "  | Available seats : "
             << left << setw(20) << show.getAvailableCount() << "|\n";

        ostringstream occ;
        occ << fixed << setprecision(2) << show.getOccupancyPct() << " %";
        cout << "  | Occupancy       : "
             << left << setw(20) << occ.str() << "|\n";

        cout << "  | Total bookings  : "
             << left << setw(20) << bookings.size() << "|\n";
        cout << " \n";
    }

public:
    TheatreSystem() : show(5, 10), bookingCounter(0) {
        loadFromFile();
    }

    // ---- Main menu loop ----
    void run() {
        cout << "\n========================================\n";
        cout << "    MOVIE TICKET BOOKING SYSTEM\n";
        cout << "========================================\n";

        bool running = true;
        while (running) {
            cout << "\n  ---- MAIN MENU ----\n";
            cout << "  1. Display seat map\n";
            cout << "  2. Make a booking\n";
            cout << "  3. Search booking by ID\n";
            cout << "  4. Occupancy report\n";
            cout << "  5. Exit\n";
            cout << "  Your choice: ";

            int choice;
            if (!(cin >> choice)) {
                // Non-integer input
                clearInputBuffer();
                cout << "  Invalid input. Please enter a number (1-5).\n";
                continue;
            }

            switch (choice) {
                case 1: menuDisplaySeats();  break;
                case 2: menuMakeBooking();   break;
                case 3: menuSearchBooking(); break;
                case 4: menuReport();        break;
                case 5:
                    cout << "\n  Goodbye! All bookings have been saved to "
                         << FILE_NAME << ".\n\n";
                    running = false;
                    break;
                default:
                    cout << "  Invalid choice. Please enter a number between 1 and 5.\n";
            }
        }
    }
};

int main() {
    TheatreSystem theatre;
    theatre.run();
    return 0;
}