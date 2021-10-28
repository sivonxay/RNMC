#include <sqlite3.h>
#include <string>
#include <utility>
#include <vector>
#include <functional>


class SqlConnection {
private:
    sqlite3 *connection;
public:
    std::string database_file_path;

    // method for executing standalone sql statements.
    // for reading and writing data, use SqlReader or SqlWriter classes.
    void exec(std::string statement) {
        sqlite3_exec(
            connection,
            statement.c_str(),
            nullptr,
            nullptr,
            nullptr);
    };

    SqlConnection(std::string database_file_path) :
        database_file_path{database_file_path} {
            sqlite3_open(database_file_path.c_str(), &connection);
    };

    ~SqlConnection() {
        sqlite3_close(connection);
    };

    // no copy constructor because we don't have access to internal state
    // of a sql connection.
    SqlConnection(SqlConnection &other) = delete;

    // move constructor
    SqlConnection(SqlConnection &&other) :
        connection{std::exchange(other.connection, nullptr)},
        database_file_path{std::move(other.database_file_path)} {};

    // no copy assignment because we don't have access to internal state
    // of a sql connection.
    SqlConnection &operator=(SqlConnection &other) = delete;

    // move assignment
    SqlConnection &operator=(SqlConnection &&other) {
        std::swap(connection, other.connection);
        std::swap(database_file_path, other.database_file_path);
        return *this;
    };
};

// Row structs correspond to rows in our sqlite databases.
// The setters attribute is a static vector of functions which
// we can call to set the corresponding attributes in the Row struct
// according to the column numbers (which are used by the sqlite API)
struct ReactionRow {
    int reaction_id;
    int number_of_reactants;
    int number_of_products;
    int reactant_1;
    int reactant_2;
    int product_1;
    int product_2;
    double rate;


    static std::vector<
        std::function<
            void(
                ReactionRow&,
                sqlite3_stmt*,
                int
                )>> setters;
};

std::vector<std::function<
                void(
                    ReactionRow&,
                    sqlite3_stmt*,
                    int)>>
ReactionRow::setters = {

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.reaction_id = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.number_of_reactants = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.number_of_products = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.reactant_1 = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.reactant_2 = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.product_1 = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.product_2 = sqlite3_column_int(stmt, i);
    },

    [](ReactionRow &r, sqlite3_stmt *stmt, int i) {
        r.rate = sqlite3_column_double(stmt, i);
    }
};
