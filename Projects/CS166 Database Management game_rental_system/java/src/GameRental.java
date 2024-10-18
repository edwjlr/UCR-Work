/*
 * Template JAVA User Interface
 * =============================
 *
 * Database Management Systems
 * Department of Computer Science &amp; Engineering
 * University of California - Riverside
 *
 * Target DBMS: 'Postgres'
 *
 */


import java.sql.DriverManager;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.List;
import java.util.ArrayList;
import java.lang.Math;

/**
 * This class defines a simple embedded SQL utility class that is designed to
 * work with PostgreSQL JDBC drivers.
 *
 */
public class GameRental {

   // reference to physical database connection.
   private Connection _connection = null;

   // handling the keyboard inputs through a BufferedReader
   // This variable can be global for convenience.
   static BufferedReader in = new BufferedReader(
                                new InputStreamReader(System.in));

   /**
    * Creates a new instance of GameRental store
    *
    * @param hostname the MySQL or PostgreSQL server hostname
    * @param database the name of the database
    * @param username the user name used to login to the database
    * @param password the user login password
    * @throws java.sql.SQLException when failed to make a connection.
    */
   public GameRental(String dbname, String dbport, String user, String passwd) throws SQLException {

      System.out.print("Connecting to database...");
      try{
         // constructs the connection URL
         String url = "jdbc:postgresql://localhost:" + dbport + "/" + dbname;
         System.out.println ("Connection URL: " + url + "\n");

         // obtain a physical connection
         this._connection = DriverManager.getConnection(url, user, passwd);
         System.out.println("Done");
      }catch (Exception e){
         System.err.println("Error - Unable to Connect to Database: " + e.getMessage() );
         System.out.println("Make sure you started postgres on this machine");
         System.exit(-1);
      }//end catch
   }//end GameRental

   /**
    * Method to execute an update SQL statement.  Update SQL instructions
    * includes CREATE, INSERT, UPDATE, DELETE, and DROP.
    *
    * @param sql the input SQL string
    * @throws java.sql.SQLException when update failed
    */
   public void executeUpdate (String sql) throws SQLException {
      // creates a statement object
      Statement stmt = this._connection.createStatement ();

      // issues the update instruction
      stmt.executeUpdate (sql);

      // close the instruction
      stmt.close ();
   }//end executeUpdate

   /**
    * Method to execute an input query SQL instruction (i.e. SELECT).  This
    * method issues the query to the DBMS and outputs the results to
    * standard out.
    *
    * @param query the input query string
    * @return the number of rows returned
    * @throws java.sql.SQLException when failed to execute the query
    */
   public int executeQueryAndPrintResult (String query) throws SQLException {
      // creates a statement object
      Statement stmt = this._connection.createStatement ();

      // issues the query instruction
      ResultSet rs = stmt.executeQuery (query);

      /*
       ** obtains the metadata object for the returned result set.  The metadata
       ** contains row and column info.
       */
      ResultSetMetaData rsmd = rs.getMetaData ();
      int numCol = rsmd.getColumnCount ();
      int rowCount = 0;

      // iterates through the result set and output them to standard out.
      boolean outputHeader = true;
      while (rs.next()){
		 if(outputHeader){
			for(int i = 1; i <= numCol; i++){
			System.out.print(rsmd.getColumnName(i) + "\t");
			}
			System.out.println();
			outputHeader = false;
		 }
         for (int i=1; i<=numCol; ++i)
            System.out.print (rs.getString (i) + "\t");
         System.out.println ();
         ++rowCount;
      }//end while
      stmt.close();
      return rowCount;
   }//end executeQuery

   /**
    * Method to execute an input query SQL instruction (i.e. SELECT).  This
    * method issues the query to the DBMS and returns the results as
    * a list of records. Each record in turn is a list of attribute values
    *
    * @param query the input query string
    * @return the query result as a list of records
    * @throws java.sql.SQLException when failed to execute the query
    */
   public List<List<String>> executeQueryAndReturnResult (String query) throws SQLException {
      // creates a statement object
      Statement stmt = this._connection.createStatement ();

      // issues the query instruction
      ResultSet rs = stmt.executeQuery (query);

      /*
       ** obtains the metadata object for the returned result set.  The metadata
       ** contains row and column info.
       */
      ResultSetMetaData rsmd = rs.getMetaData ();
      int numCol = rsmd.getColumnCount ();
      int rowCount = 0;

      // iterates through the result set and saves the data returned by the query.
      boolean outputHeader = false;
      List<List<String>> result  = new ArrayList<List<String>>();
      while (rs.next()){
        List<String> record = new ArrayList<String>();
		for (int i=1; i<=numCol; ++i)
			record.add(rs.getString (i));
        result.add(record);
      }//end while
      stmt.close ();
      return result;
   }//end executeQueryAndReturnResult

   /**
    * Method to execute an input query SQL instruction (i.e. SELECT).  This
    * method issues the query to the DBMS and returns the number of results
    *
    * @param query the input query string
    * @return the number of rows returned
    * @throws java.sql.SQLException when failed to execute the query
    */
   public int executeQuery (String query) throws SQLException {
       // creates a statement object
       Statement stmt = this._connection.createStatement ();

       // issues the query instruction
       ResultSet rs = stmt.executeQuery (query);

       int rowCount = 0;

       // iterates through the result set and count nuber of results.
       while (rs.next()){
          rowCount++;
       }//end while
       stmt.close ();
       return rowCount;
   }

   /**
    * Method to fetch the last value from sequence. This
    * method issues the query to the DBMS and returns the current
    * value of sequence used for autogenerated keys
    *
    * @param sequence name of the DB sequence
    * @return current value of a sequence
    * @throws java.sql.SQLException when failed to execute the query
    */
   public int getCurrSeqVal(String sequence) throws SQLException {
	Statement stmt = this._connection.createStatement ();

	ResultSet rs = stmt.executeQuery (String.format("Select currval('%s')", sequence));
	if (rs.next())
		return rs.getInt(1);
	return -1;
   }

   /**
    * Method to close the physical connection if it is open.
    */
   public void cleanup(){
      try{
         if (this._connection != null){
            this._connection.close ();
         }//end if
      }catch (SQLException e){
         // ignored.
      }//end try
   }//end cleanup

   /**
    * The main execution method
    *
    * @param args the command line arguments this inclues the <mysql|pgsql> <login file>
    */
   public static void main (String[] args) {
      if (args.length != 3) {
         System.err.println (
            "Usage: " +
            "java [-classpath <classpath>] " +
            GameRental.class.getName () +
            " <dbname> <port> <user>");
         return;
      }//end if

      Greeting();
      GameRental esql = null;
      try{
         // use postgres JDBC driver.
         Class.forName ("org.postgresql.Driver").newInstance ();
         // instantiate the GameRental object and creates a physical
         // connection.
         String dbname = args[0];
         String dbport = args[1];
         String user = args[2];
         esql = new GameRental (dbname, dbport, user, "");

         boolean keepon = true;
         while(keepon) {
            // These are sample SQL statements
            System.out.println("MAIN MENU");
            System.out.println("---------");
            System.out.println("1. Create user");
            System.out.println("2. Log in");
            System.out.println("9. < EXIT");
            String authorisedUser = null;
            String userRole = null;
            switch (readChoice()){
               case 1: CreateUser(esql); break;
               case 2:
                  authorisedUser = LogIn(esql);
                  if (authorisedUser != null) {
                     // Get the user's role
                     String roleQuery = String.format("SELECT role FROM Users WHERE login = '%s'", authorisedUser);
                     List<List<String>> roleResult = esql.executeQueryAndReturnResult(roleQuery);
                     if (!roleResult.isEmpty()) {
                           userRole = roleResult.get(0).get(0).trim();
                     }
                  }
                  break;
               case 9: keepon = false; break;
               default : System.out.println("Unrecognized choice!"); break;
            }//end switch
            if (authorisedUser != null) {
              boolean usermenu = true;
              while(usermenu) {
                System.out.println("MAIN MENU");
                System.out.println("---------");
                System.out.println("1. View Profile");
                System.out.println("2. Update Profile");
                System.out.println("3. View Catalog");
                System.out.println("4. Place Rental Order");
                System.out.println("5. View Full Rental Order History");
                System.out.println("6. View Past 5 Rental Orders");
                System.out.println("7. View Rental Order Information");
                System.out.println("8. View Tracking Information");

               // Show additional options for employees and managers
               if (userRole != null && (userRole.equals("employee") || userRole.equals("manager"))) {
                  System.out.println("9. Update Tracking Information");
               }
               if (userRole != null && userRole.equals("manager")) {
                  System.out.println("10. Update Catalog");
                  System.out.println("11. Update User");
               }

               System.out.println(".........................");
               System.out.println("20. Log out");
                switch (readChoice()){
                   case 1: viewProfile(esql, authorisedUser); break;
                   case 2: updateProfile(esql, authorisedUser); break;
                   case 3: viewCatalog(esql); break;
                   case 4: placeOrder(esql, authorisedUser); break;
                   case 5: viewAllOrders(esql, authorisedUser); break;
                   case 6: viewRecentOrders(esql, authorisedUser); break;
                   case 7: viewOrderInfo(esql, authorisedUser); break;
                   case 8: viewTrackingInfo(esql); break;

                  case 9:
                     if (userRole != null && (userRole.equals("employee") || userRole.equals("manager"))) {
                        updateTrackingInfo(esql);
                     } else {
                        System.out.println("Unrecognized choice!");
                     }
                     break;
                  case 10:
                     if (userRole != null && userRole.equals("manager")) {
                        updateCatalog(esql);
                     } else {
                        System.out.println("Unrecognized choice!");
                     }
                     break;
                  case 11:
                     if (userRole != null && userRole.equals("manager")) {
                        updateUser(esql);
                     } else {
                        System.out.println("Unrecognized choice!");
                     }
                     break;

                   case 20: usermenu = false; break;
                   default : System.out.println("Unrecognized choice!"); break;
                }
              }
            }
         }//end while
      }catch(Exception e) {
         System.err.println (e.getMessage ());
      }finally{
         // make sure to cleanup the created table and close the connection.
         try{
            if(esql != null) {
               System.out.print("Disconnecting from database...");
               esql.cleanup ();
               System.out.println("Done\n\nBye !");
            }//end if
         }catch (Exception e) {
            // ignored.
         }//end try
      }//end try
   }//end main

   public static void Greeting(){
      System.out.println(
         "\n\n*******************************************************\n" +
         "              User Interface      	               \n" +
         "*******************************************************\n");
   }//end Greeting

   /*
    * Reads the users choice given from the keyboard
    * @int
    **/
   public static int readChoice() {
      int input;
      // returns only if a correct value is given.
      do {
         System.out.print("Please make your choice: ");
         try { // read the integer, parse it and break.
            input = Integer.parseInt(in.readLine());
            break;
         }catch (Exception e) {
            System.out.println("Your input is invalid!");
            continue;
         }//end try
      }while (true);
      return input;
   }//end readChoice

   /*
    * Creates a new user
    **/
   public static void CreateUser(GameRental esql) {
      try {
         System.out.print("Enter username: ");
         String login = in.readLine();
         System.out.print("Enter password: ");
         String password = in.readLine();
         System.out.print("Enter phone number: ");
         String phoneNum = in.readLine();

         String query = String.format(
               "INSERT INTO Users (login, password, role, favGames, phoneNum, numOverDueGames) VALUES ('%s', '%s', 'customer', '', '%s', 0)",
               login, password, phoneNum);
         esql.executeUpdate(query);
         System.out.println("User registered successfully.");
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }//end CreateUser


   /*
    * Check log in credentials for an existing user
    * @return User login or null is the user does not exist
    **/
   public static String LogIn(GameRental esql){
      try {
         System.out.print("Enter username: ");
         String login = in.readLine();
         System.out.print("Enter password: ");
         String password = in.readLine();

         String query = String.format("SELECT * FROM Users WHERE login = '%s' AND password = '%s'", login, password);
         int userNum = esql.executeQuery(query);
         if (userNum > 0)
            return login;
         else {
            System.out.println("Invalid username or password.");
            return null;
         }
      } catch (Exception e) {
         System.err.println(e.getMessage());
         return null;
      }
   }//end

// Rest of the functions definition go in here
   public static void viewProfile(GameRental esql, String authorisedUser) {
      try {
         String query = String.format("SELECT * FROM Users WHERE login = '%s'", authorisedUser);
         esql.executeQueryAndPrintResult(query);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void updateProfile(GameRental esql, String authorisedUser) {
      try {
         System.out.println("Update Profile Menu");
         System.out.println("1. Update Password");
         System.out.println("2. Update Favorite Games");
         System.out.println("3. Update Phone Number");
         System.out.println("9. Go Back");

         switch (readChoice()) {
               case 1:
                  System.out.print("Enter your new password: ");
                  String newPassword = in.readLine();
                  String passwordQuery = String.format("UPDATE Users SET password = '%s' WHERE login = '%s'", newPassword, authorisedUser);
                  esql.executeUpdate(passwordQuery);
                  System.out.println("Password updated successfully.");
                  break;
               case 2:
                  System.out.print("Enter your favorite games (comma-separated): ");
                  String favoriteGames = in.readLine();
                  String favGamesQuery = String.format("UPDATE Users SET favGames = '%s' WHERE login = '%s'", favoriteGames, authorisedUser);
                  esql.executeUpdate(favGamesQuery);
                  System.out.println("Favorite games updated successfully.");
                  break;
               case 3:
                  System.out.print("Enter your new phone number: ");
                  String phoneNumber = in.readLine();
                  String phoneQuery = String.format("UPDATE Users SET phoneNum = '%s' WHERE login = '%s'", phoneNumber, authorisedUser);
                  esql.executeUpdate(phoneQuery);
                  System.out.println("Phone number updated successfully.");
                  break;
               case 9:
                  return;
               default:
                  System.out.println("Unrecognized choice!");
                  break;
         }
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }


   public static void viewCatalog(GameRental esql) {
      try {
         // Query and print available genres
         String genreQuery = "SELECT DISTINCT genre FROM Catalog";
         List<List<String>> genres = esql.executeQueryAndReturnResult(genreQuery);
         System.out.println("Available genres:");
         for (List<String> genre : genres) {
               System.out.println("- " + genre.get(0));
         }

         System.out.print("Enter genre (or leave empty for all): ");
         String genre = in.readLine();
         System.out.print("Enter maximum price (or leave empty for no limit): ");
         String priceInput = in.readLine();
         double maxPrice = priceInput.isEmpty() ? Double.MAX_VALUE : Double.parseDouble(priceInput);

         String query = "SELECT * FROM Catalog";
         boolean whereAdded = false;

         if (!genre.isEmpty()) {
               query += String.format(" WHERE genre = '%s'", genre);
               whereAdded = true;
         }
         if (maxPrice != Double.MAX_VALUE) {
               query += whereAdded ? " AND" : " WHERE";
               query += String.format(" price <= %.2f", maxPrice);
         }

         query += " ORDER BY price ASC"; // or DESC for highest to lowest price
         esql.executeQueryAndPrintResult(query);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void placeOrder(GameRental esql, String authorisedUser) {
      try {
         System.out.print("Enter game IDs to rent (comma-separated): ");
         String gameIDs = in.readLine();
         String[] games = gameIDs.split(",");

         int noOfGames = games.length;
         double totalPrice = 0.0;

         // Calculate total price of the games
         for (String gameID : games) {
            String query = String.format("SELECT price FROM Catalog WHERE gameID = '%s'", gameID.trim());
            List<List<String>> data = esql.executeQueryAndReturnResult(query);
            if (!data.isEmpty()) {
               totalPrice += Double.parseDouble(data.get(0).get(0));
            } else {
               System.out.println("Game ID " + gameID.trim() + " not found in catalog.");
               return;
            }
         }

         // Generate rentalOrderID and trackingID
         String rentalOrderID = java.util.UUID.randomUUID().toString();
         String trackingID = java.util.UUID.randomUUID().toString();

         // Insert new rental order
         String insertOrder = String.format("INSERT INTO RentalOrder (rentalOrderID, login, noOfGames, totalPrice, orderTimestamp, dueDate) VALUES ('%s', '%s', %d, %.2f, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP + INTERVAL '7 days')",
            rentalOrderID, authorisedUser, noOfGames, totalPrice);
         esql.executeUpdate(insertOrder);

         // Insert tracking information
         String insertTrackingInfo = String.format("INSERT INTO TrackingInfo (trackingID, rentalOrderID, status, currentLocation, courierName, lastUpdateDate, additionalComments) VALUES ('%s', '%s', 'Processing', 'Warehouse', 'Courier Service', CURRENT_TIMESTAMP, '')",
            trackingID, rentalOrderID);
         esql.executeUpdate(insertTrackingInfo);

         // Insert games into gamesinorder
         for (String gameID : games) {
            String insertGameInOrder = String.format("INSERT INTO GamesInOrder (rentalOrderID, gameID, unitsOrdered) VALUES ('%s', '%s', 1)", rentalOrderID, gameID.trim());
            esql.executeUpdate(insertGameInOrder);
         }

         System.out.println("Rental order placed successfully.");
         System.out.println("Tracking ID: " + trackingID);

        // Check the effect of the trigger
        String triggerCheckQuery = String.format("SELECT numOverDueGames FROM Users WHERE login = '%s'", authorisedUser);
        esql.executeQueryAndPrintResult(triggerCheckQuery);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }


   public static void viewAllOrders(GameRental esql, String authorisedUser) {
      try {
         String query = String.format("SELECT * FROM RentalOrder WHERE login = '%s'", authorisedUser);
         esql.executeQueryAndPrintResult(query);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void viewRecentOrders(GameRental esql, String authorisedUser) {
      try {
         String query = String.format("SELECT * FROM RentalOrder WHERE login = '%s' ORDER BY orderTimestamp DESC LIMIT 5", authorisedUser);
         esql.executeQueryAndPrintResult(query);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void viewOrderInfo(GameRental esql, String authorisedUser) {
      try {
         System.out.print("Enter rental order ID: ");
         String orderID = in.readLine();

         // Query to get order details
         String orderQuery = String.format("SELECT * FROM RentalOrder WHERE rentalOrderID = '%s' AND login = '%s'", orderID, authorisedUser);
         System.out.println("Order Details:");
         int orderCount = esql.executeQueryAndPrintResult(orderQuery);
         if (orderCount == 0) {
            System.out.println("No order details found for the given rental order ID.");
            return;
         }

         // Query to get tracking information
         String trackingQuery = String.format("SELECT * FROM TrackingInfo WHERE rentalOrderID = '%s'", orderID);
         System.out.println("Tracking Information:");
         int trackingCount = esql.executeQueryAndPrintResult(trackingQuery);
         if (trackingCount == 0) {
            System.out.println("No tracking information found for the given rental order ID.");
         }

         // Query to get games in order
         String gamesQuery = String.format("SELECT * FROM GamesInOrder WHERE rentalOrderID = '%s'", orderID);
         System.out.println("Games in Order:");
         int gamesCount = esql.executeQueryAndPrintResult(gamesQuery);
         if (gamesCount == 0) {
            System.out.println("No games found for the given rental order ID.");
         }
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void viewTrackingInfo(GameRental esql) {
      try {
         System.out.print("Enter tracking ID: ");
         String trackingID = in.readLine();

         String query = String.format("SELECT * FROM TrackingInfo WHERE trackingID = '%s'", trackingID);
         esql.executeQueryAndPrintResult(query);
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void updateTrackingInfo(GameRental esql) {
      try {
         System.out.print("Enter tracking ID: ");
         String trackingID = in.readLine();
         System.out.print("Enter new status: ");
         String status = in.readLine();
         System.out.print("Enter new location: ");
         String location = in.readLine();

         String query = String.format("UPDATE TrackingInfo SET status = '%s', currentLocation = '%s', lastUpdateDate = CURRENT_TIMESTAMP WHERE trackingID = '%s'", status, location, trackingID);
         esql.executeUpdate(query);
         System.out.println("Tracking information updated successfully.");
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void updateCatalog(GameRental esql) {
      try {
         System.out.print("Enter game ID to update: ");
         String gameID = in.readLine();
         System.out.print("Enter new price: ");
         String newPrice = in.readLine();

         String query = String.format("UPDATE Catalog SET price = '%s' WHERE gameID = '%s'", newPrice, gameID);
         esql.executeUpdate(query);
         System.out.println("Game information updated successfully.");
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

   public static void updateUser(GameRental esql) {
      try {
         System.out.print("Enter username to update: ");
         String username = in.readLine();
         System.out.print("Enter new role (customer/employee/manager): ");
         String newRole = in.readLine();
         System.out.print("Enter new numOverdueGames: ");
         String numOverdueGames = in.readLine();

         String query = String.format("UPDATE Users SET role = '%s', numOverDueGames = %s WHERE login = '%s'", newRole, numOverdueGames, username);
         esql.executeUpdate(query);
         System.out.println("User information updated successfully.");
      } catch (Exception e) {
         System.err.println(e.getMessage());
      }
   }

}//end GameRental