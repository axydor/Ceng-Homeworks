import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.List;
import java.sql.Statement;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.util.ArrayList;

/**
 *
 * @author gorkem
 */
public class MovieManager implements IMovieManager {

    /* These values are read from databaseConfiguration.txt file. */
    private String _USERNAME;
    private String _PASSWORD;
    private String _HOST;
    private String _DATABASE_SCHEMA;
    private int _PORT_NUMBER;

    private static String _DB_CONFIG_FILE_NAME = "databaseConfiguration.txt";
    private static String _ACTOR_FILE_NAME = "actors.txt";
    private static String _MOVIE_FILE_NAME = "movies.txt";
    private static String _DIRECTOR_FILE = "directors.txt";
    private static String _USER_FILE = "users.txt";

    private Connection _connection;
    private PreparedStatement stmt;
    // DO NOT MODIFY THIS FUNCTION
    public void ReadDatabaseConfiguration() throws Exception {
        BufferedReader br = null;

        try {
            String currentLine;
            br = new BufferedReader(new FileReader(this._DB_CONFIG_FILE_NAME));

            while ((currentLine = br.readLine()) != null) {
                if (currentLine.startsWith("host")) {
                    this._HOST = currentLine.replace("host=", "").trim();
                }
                if (currentLine.startsWith("schema")) {
                    this._DATABASE_SCHEMA = currentLine.replace("schema=", "").trim();
                }
                if (currentLine.startsWith("username")) {
                    this._USERNAME = currentLine.replace("username=", "").trim();
                }
                if (currentLine.startsWith("password")) {
                    this._PASSWORD = currentLine.replace("password=", "").trim();
                }
                if (currentLine.startsWith("port")) {
                    this._PORT_NUMBER = Integer.parseInt(currentLine.replace("port=", "").trim());
                }
            }
        } catch (Exception ex) {
            System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
            throw ex;
        } finally {
            try {
                if (br != null) {
                    br.close();
                }
            } catch (IOException ex) {
                System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
                throw ex;
            }
        }

        System.out.println("Database configuration is read.");
    }

    // DO NOT MODIFY THIS FUNCTION
    public void InitializeConnection() {

        String url = "jdbc:mysql://" + this._HOST + ":" + this._PORT_NUMBER + "/"
                + this._DATABASE_SCHEMA + "?useUnicode=true&characterEncoding=UTF-8";

        try {
            Class.forName("com.mysql.jdbc.Driver");
            this._connection = DriverManager.getConnection(url, this._USERNAME,
                    this._PASSWORD);
        } catch (SQLException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {

            e.printStackTrace();
        }

        System.out.println("Database connection initialized.");
    }

    @Override
    public void ParseFilesAndInsertData() {
        BufferedReader br = null;
        FileReader fr = null;
        stmt = null;
        try {
            fr = new FileReader("actors.txt");
            br = new BufferedReader(fr);
            String line;
            while ((line = br.readLine()) != null) {
                String [] arr = line.split(";");
                try{
                    String query = "INSERT INTO actor(aid,name) VALUES(?,?)";
                    stmt = _connection.prepareStatement(query);
                    stmt.setInt(1,Integer.valueOf(arr[0]));
                    stmt.setString(2,arr[1]);
                    stmt.executeUpdate();
                    if (stmt != null)
                        stmt.close();
                } catch (SQLException e){
                    System.out.println(e.getMessage());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (br != null)
                    br.close();
                if (fr != null)
                    fr.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
         }
        try {
            fr = new FileReader("directors.txt");
            br = new BufferedReader(fr);
            String line;
            while ((line = br.readLine()) != null) {
                String [] arr = line.split(";");
                try{
                    String query = "INSERT INTO director(did,name) VALUES(?,?)";
                    stmt = _connection.prepareStatement(query);
                    stmt.setInt(1,Integer.valueOf(arr[0]));
                    stmt.setString(2,arr[1]);
                    stmt.executeUpdate();
                    if (stmt != null)
                        stmt.close();
                } catch (SQLException e){
                    System.out.println(e.getMessage());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (br != null)
                    br.close();
                if (fr != null)
                    fr.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
         } 
         
        try {
            fr = new FileReader("movies.txt");
            br = new BufferedReader(fr);
            String line;
            while ((line = br.readLine()) != null) {
                int did = 0;
                String [] arr = line.split(";");
                try{
                    String query = "INSERT INTO movie(mid,title,genre,year,did,rating) VALUES(?,?,?,?,?,?)";
                    try{         // GETTING THE DIRECTOR ID 
                        String direc = "SELECT * FROM director WHERE name = ?";
                        stmt = _connection.prepareStatement(direc);
                        stmt.setString(1,arr[5]);
                        ResultSet rs = stmt.executeQuery();
                        while(rs.next()){
                            did = rs.getInt("did");
                        }
                        if (stmt != null)
                            stmt.close();
                    } catch(SQLException e){
                        System.out.println(e.getMessage());
                    }
                    stmt = _connection.prepareStatement(query);
                    stmt.setInt(1,Integer.valueOf(arr[0]));
                    stmt.setString(2,arr[1]);
                    stmt.setString(3,arr[3]);
                    stmt.setInt(4,Integer.valueOf(arr[2]));
                    stmt.setInt(5,did);
                    stmt.setDouble(6,Double.parseDouble(arr[6])); 
                    stmt.executeUpdate();
                    if( stmt != null)
                        stmt.close();

                    // INSERTING INTO THE CASTED_IN TABLE  

                    String [] actors = arr[4].split(", ");   // FUCK THE EMPTY LINE ON THE LEFT 

                    for (int i = 0; i < 4 ; i++){
                        ResultSet rs = null;
                        int aid = 0;
                        String cast_in = "INSERT INTO casted_in(aid,mid) VALUES(?,?)";
                        try{
                            String actor = "SELECT * FROM actor WHERE name = ?"; 
                            stmt = _connection.prepareStatement(actor);
                            stmt.setString(1,actors[i]);
                            rs = stmt.executeQuery();
                            if(rs.next());
                                aid = rs.getInt("aid");
                            if (stmt != null)
                                stmt.close();

                        } catch(SQLException e){
                              System.out.println(e.getMessage());
                        } 
                        stmt = _connection.prepareStatement(cast_in);
                        stmt.setInt(1,aid);
                        stmt.setInt(2,Integer.valueOf(arr[0]));
                        stmt.executeUpdate();
                        if (stmt != null)
                            stmt.close();
                    }
                } catch (SQLException e){
                    System.out.println(e.getMessage());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (br != null)
                    br.close();
                if (fr != null)
                    fr.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
         }    

        try {
            //br = new BufferedReader(new FileReader(FILENAME));
            fr = new FileReader("users.txt");
            br = new BufferedReader(fr);
            String line;
            while ((line = br.readLine()) != null) {
                String [] arr = line.split(";");
                try{
                    String query = "INSERT INTO user(uid,name,age) VALUES(?,?,?)";
                    stmt = _connection.prepareStatement(query);
                    stmt.setInt(1,Integer.valueOf(arr[0]));
                    stmt.setString(2,arr[1]);
                    stmt.setInt(3,Integer.parseInt(arr[2]));
                    stmt.executeUpdate();

                    // INSERTING INTO THE WACTHED 
                    String [] movies = arr[3].split(", ");
                    try{
                        for(int i = 0; i < 10; i++){
                            String watched = "INSERT INTO watched(uid,mid) VALUES(?,?)";
                            stmt = _connection.prepareStatement(watched);
                            stmt.setInt(1,Integer.parseInt(arr[0]));
                            stmt.setInt(2,Integer.parseInt(movies[i]));
                            stmt.executeUpdate();
                            if(stmt != null)
                                stmt.close();
                        }
                            if (stmt != null)
                            stmt.close();
                    } catch(SQLException e){
                        System.out.println(e.getMessage());
                    }
                } catch (SQLException e){
                    System.out.println(e.getMessage());
                }
            }
        }  catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (br != null)
                    br.close();
                if (fr != null)
                    fr.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
         }
    }

    @Override
    public void CreateTables() {
        stmt = null;
        String actor_table      = "CREATE TABLE actor(" 
                                  +"  aid integer, "
                                  +"  name VARCHAR(50),"
                                  +"   PRIMARY KEY (aid) )";
        String director_table   = "CREATE TABLE director("
                                  +"  did integer," 
                                  +"  name VARCHAR(50),"
                                  +"  PRIMARY KEY (did) )";
        String movie_table      = "CREATE TABLE movie("
                                  +"  mid integer," 
                                  +"  title VARCHAR(50), "
                                  +"  genre VARCHAR(50), " 
                                  +"  year integer, "
                                  +"  did integer NOT NULL, "
                                  +"  rating DOUBLE,"
                                  +"  PRIMARY KEY(mid),"
                                  +"  FOREIGN KEY (did) REFERENCES director(did) ON DELETE CASCADE)";
        String user_table       = "CREATE TABLE user("
                                  +"  uid integer, "
                                  +"  name VARCHAR(50), "
                                  +"  age integer,"
                                  +"  PRIMARY KEY(uid))";
        String casted_in_table  = "CREATE TABLE casted_in("
                                  +"  aid integer, "
                                  +"  mid integer,"
                                  +"  PRIMARY KEY (aid,mid),"
                                  +"  FOREIGN KEY (aid) REFERENCES actor(aid) ON DELETE CASCADE,"
                                  +"  FOREIGN KEY (mid) REFERENCES movie(mid) ON DELETE CASCADE)";
        String watched_table    = "CREATE TABLE watched("
                                  +"  uid integer, mid integer,"
                                  +"  PRIMARY KEY(uid,mid),"
                                  +"  FOREIGN KEY (uid) REFERENCES user(uid) ON DELETE CASCADE,"
                                  +"  FOREIGN KEY (mid) REFERENCES movie(mid) ON DELETE CASCADE)";

        try{
            stmt = _connection.prepareStatement(actor_table);
            stmt.executeUpdate();
            stmt = _connection.prepareStatement(director_table);
            stmt.executeUpdate();
            stmt = _connection.prepareStatement(movie_table);
            stmt.executeUpdate();
            stmt = _connection.prepareStatement(user_table);
            stmt.executeUpdate();
            stmt = _connection.prepareStatement(casted_in_table);
            stmt.executeUpdate();
            stmt = _connection.prepareStatement(watched_table);
            stmt.executeUpdate();

        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertActor(Actor actor) {
        stmt = null;
        String query = "INSERT INTO actor(aid,name) VALUES (?,?)";
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,actor.getAid());
            stmt.setString(2,actor.getName());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertDirector(Director director) {
        stmt = null;
        String query = "INSERT INTO director(did,name) VALUES (?,?)" ;
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,director.getDid());
            stmt.setString(2,director.getName());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertMovie(Movie movie) {
        stmt = null;
        String query = "INSERT INTO movie(mid, title, genre, rating, year, did) "
                                      +        "VALUES (?,?,?,?,?,?)";
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,movie.getMid());
            stmt.setString(2,movie.getTitle());
            stmt.setString(3,movie.getGenre());
            stmt.setFloat(4,movie.getRating());
            stmt.setFloat(5,movie.getDirector().getDid());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertUser(User user) {
        stmt = null;
        String query = "INSERT INTO user(uid,name,age) VALUES (?,?,?)";
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,user.getUid());
            stmt.setString(2,user.getName());
            stmt.setInt(3,user.getAge());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertCastedIn(Actor actor, Movie movie) {
        stmt = null;
        String query = "INSERT INTO casted_in(aid,mid) VALUES (?,?)";
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,actor.getAid());
            stmt.setInt(2,movie.getMid());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void InsertWatched(User user, Movie movie) {
        stmt = null;
        String query = "INSERT INTO watched(uid,mid) VALUES (?,?)";
        try{
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,user.getUid());
            stmt.setInt(2,movie.getMid());
            stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try{
                if ( stmt != null )
                    stmt.close();
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public List<Movie> GetMoviesOfDirector(int did) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "SELECT * FROM movie WHERE did = ?";
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,did);
            ResultSet rs = stmt.executeQuery();

            String dir = "SELECT * FROM director WHERE did = ?";
            stmt = _connection.prepareStatement(dir);
            stmt.setInt(1,did);
            ResultSet direc = stmt.executeQuery();
            direc.next();
            Director director = new Director(did,direc.getString("name"));
            while(rs.next()) {
                Movie movie = new Movie(rs.getInt("mid"),rs.getString("title"),rs.getString("genre"),rs.getFloat("rating"),rs.getInt("year"),director);
                movies.add(movie);
            }
        } catch(SQLException e) {
            System.out.println(e.getMessage());
        } finally {
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }   
        if( movies.size() == 0)
            return null;
        return movies;
    }

    @Override
    public List<Movie> GetMoviesOfActor(int aid) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "SELECT * FROM casted_in WHERE aid = ?";
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,aid);
            ResultSet rs = stmt.executeQuery();
            //FINDING MOVIES BY MID
            while(rs.next()) {
                String one_movie = "SELECT * FROM movie WHERE mid = ?"; // SELECTING ONE MOVIE
                stmt = _connection.prepareStatement(one_movie);
                stmt.setInt(1,rs.getInt("mid"));
                ResultSet movs = stmt.executeQuery();

                while(movs.next()){
                    String dir = "SELECT * FROM director WHERE did = ?";  // GETTING THE DIRECTOR OF THAT MOVIE
                    stmt = _connection.prepareStatement(dir);
                    stmt.setInt(1,movs.getInt("did"));
                    ResultSet direc = stmt.executeQuery();
                    direc.next();
                    Director director = new Director(movs.getInt("did"),direc.getString("name"));                
                    Movie movie = new Movie(movs.getInt("mid"),movs.getString("title"),movs.getString("genre"),movs.getFloat("rating"),movs.getInt("year"),director);
                    movies.add(movie);
                }
            }
        } catch(SQLException e) {
            System.out.println(e.getMessage());
        } finally {
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }   
        if( movies.size() == 0)
            return null;
        return movies;
    }

    @Override
    public List<Movie> GetUserWatchlist(int uid) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "SELECT * FROM watched WHERE uid = ?";
            stmt = _connection.prepareStatement(query);
            stmt.setInt(1,uid);
            ResultSet rs = stmt.executeQuery();
            //FINDING MOVIES BY MID
            while(rs.next()) {
                String one_movie = "SELECT * FROM movie WHERE mid = ?"; // SELECTING ONE MOVIE
                stmt = _connection.prepareStatement(one_movie);
                stmt.setInt(1,rs.getInt("mid"));
                ResultSet movs = stmt.executeQuery();

                while(movs.next()){
                    String dir = "SELECT * FROM director WHERE did = ?";  // GETTING THE DIRECTOR OF THAT MOVIE
                    stmt = _connection.prepareStatement(dir);
                    stmt.setInt(1,movs.getInt("did"));
                    ResultSet direc = stmt.executeQuery();
                    direc.next();
                    Director director = new Director(movs.getInt("did"),direc.getString("name"));                
                    Movie movie = new Movie(movs.getInt("mid"),movs.getString("title"),movs.getString("genre"),movs.getFloat("rating"),movs.getInt("year"),director);
                    movies.add(movie);
                }
            }
        } catch(SQLException e) {
            System.out.println(e.getMessage());
        } finally {
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }   
        if( movies.size() == 0)
            return null;
        return movies;
    }

    @Override
    public List<Movie> GetMoviesWithTwoActors(String actorName1, String actorName2) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "(SELECT * FROM actor WHERE name = ?)"
                           + "UNION"
                           + "(SELECT * FROM actor WHERE name = ?)";
            stmt = _connection.prepareStatement(query);
            stmt.setString(1,actorName1);
            stmt.setString(2,actorName2);
            ResultSet rs = stmt.executeQuery();    

            int i = 1;
            int aid_1=0,aid_2=0;
            while(rs.next()){
                if (i == 1)
                    aid_1 = rs.getInt("aid");
                else if (i == 2)
                    aid_2 = rs.getInt("aid");
                i++;
            }

            String mov = "SELECT * FROM casted_in WHERE aid = ? "
                         + "AND mid IN (SELECT mid FROM casted_in WHERE aid = ?)";
            stmt = _connection.prepareStatement(mov);
            stmt.setInt(1,aid_1);
            stmt.setInt(2,aid_2);
            ResultSet movie_id = stmt.executeQuery();
                
            while(movie_id.next()){
                String just_movie = "SELECT * FROM movie WHERE mid = ?";  // GETTING THE MOVIE 
                stmt = _connection.prepareStatement(just_movie);
                stmt.setInt(1,movie_id.getInt("mid"));
                ResultSet movs = stmt.executeQuery();
                
                while(movs.next()){
                    String dir = "SELECT * FROM director WHERE did = ?";  // GETTING THE DIRECTOR OF THAT MOVIE
                    stmt = _connection.prepareStatement(dir);
                    stmt.setInt(1,movs.getInt("did"));
                    ResultSet direc = stmt.executeQuery();
                    while(direc.next()){
                        Director director = new Director(movs.getInt("did"),direc.getString("name"));                
                        Movie movie = new Movie(movs.getInt("mid"),movs.getString("title"),movs.getString("genre"),movs.getFloat("rating"),movs.getInt("year"),director);
                        movies.add(movie);
                    }
                }
            }
        } catch (SQLException e) {
            System.out.println(e.getMessage());
        } finally {
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }   
        if( movies.size() == 0)
            return null;
        return movies;
    }

    @Override
    public List<Movie> GetMoviesAboveRating(float rating) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "SELECT * from movie WHERE rating > ?";
            stmt = _connection.prepareStatement(query);
            stmt.setDouble(1,(double)rating);
            ResultSet movs = stmt.executeQuery();
            
            while(movs.next()){
                String dir = "SELECT * FROM director WHERE did = ?";  // GETTING THE DIRECTOR OF THAT MOVIE
                stmt = _connection.prepareStatement(dir);
                stmt.setInt(1,movs.getInt("did"));
                ResultSet direc = stmt.executeQuery();
                while(direc.next()){
                    Director director = new Director(movs.getInt("did"),direc.getString("name"));                
                    Movie movie = new Movie(movs.getInt("mid"),movs.getString("title"),movs.getString("genre"),movs.getFloat("rating"),movs.getInt("year"),director);
                    movies.add(movie);
                }
            }

        }catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }   
        if( movies.size() == 0)
            return null;
        return movies;
    }

    @Override
    public void ChangeRatingsOfMoviesLike(String pattern, float value) {
        List<Movie> movies = new ArrayList<Movie>();
        try{
            String query = "UPDATE movie SET rating = ? WHERE title LIKE '%' ? ";
            stmt = _connection.prepareStatement(query);
            stmt.setDouble(1,(double)value);
            stmt.setString(2,pattern);
            stmt.executeUpdate();

        }catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }       
    }

    @Override
    public void DeleteDirectorWithId(int did) {
        try{
        String query = "DELETE FROM director WHERE did = ?";
        stmt = _connection.prepareStatement(query);
        stmt.setInt(1,did);
        stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void DeleteUserWithId(int uid) {
        try{
        String query = "DELETE FROM user WHERE uid = ?";
        stmt = _connection.prepareStatement(query);
        stmt.setInt(1,uid);
        stmt.executeUpdate();
        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }
        }
    }

    @Override
    public void PrintViewStatsByMovie() {
        try{
            String query = "SELECT TABLE1.mid,TABLE2.title,TABLE1.COUNT "
                           + "FROM (SELECT mid,COUNT(*) AS COUNT FROM watched GROUP BY mid) AS TABLE1 ,"
                           + " movie AS TABLE2"
                           + " WHERE TABLE1.mid = TABLE2.mid "
                           + " ORDER BY TABLE1.COUNT DESC, TABLE2.title ASC";   
            stmt = _connection.prepareStatement(query);
            ResultSet rs = stmt.executeQuery();
            while(rs.next()){
                System.out.println(rs.getString("mid")+ "|"+ rs.getString("title")+"|"+rs.getString("COUNT"));
            }
        } catch (SQLException e) {
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }            
        }
    }

    @Override
    public void PrintViewStatsByGenre() {
         try{
            String query = "SELECT TABLE2.genre,TABLE1.COUNT "
                           + "FROM (SELECT mid,COUNT(*) AS COUNT FROM watched GROUP BY mid) AS TABLE1 ,"
                           + " movie AS TABLE2"
                           + " WHERE TABLE1.mid = TABLE2.mid "
                           + " GROUP BY TABLE2.genre,TABLE1.COUNT ORDER BY TABLE1.COUNT DESC, TABLE2.genre ASC";   
            stmt = _connection.prepareStatement(query);
            ResultSet rs = stmt.executeQuery();
            while(rs.next()){
                System.out.println(rs.getString("genre")+ "|"+ rs.getString("COUNT"));
            }
        } catch (SQLException e) {
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }            
        }       

    }

    @Override
    public void PrintAverageMovieRatingOfDirectors() {
         try{
            String query = "SELECT TABLE1.name,AVG(TABLE3.AVG),SUM(TABLE2.COUNT)"
                           +" FROM director as TABLE1, "
                           +" (SELECT mid,COUNT(*) AS COUNT FROM watched GROUP BY mid) AS TABLE2,"
                           +" (SELECT mid,did,AVG(rating) AS AVG from movie GROUP BY did,mid) AS TABLE3"
                           +" WHERE TABLE1.did = TABLE3.did AND TABLE3.mid = TABLE2.mid"
                           +" GROUP BY TABLE1.name";

            stmt = _connection.prepareStatement(query);
            ResultSet rs = stmt.executeQuery();
            while(rs.next()){
                System.out.println(rs.getString("name")+ "|"+ rs.getString("AVG(TABLE3.AVG)")+"|" + rs.getString("SUM(TABLE2.COUNT)"));
            }
        } catch (SQLException e) {
            System.out.println(e.getMessage());
        } finally{
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }            
        }       


    }

    @Override
    public void DropTables() {
        try{
            String solve_problems = "SET FOREIGN_KEY_CHECKS=0 ";
            stmt = _connection.prepareStatement(solve_problems);
            stmt.executeUpdate();            
            String query = "DROP TABLE IF EXISTS user";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();
            query = "DROP TABLE IF EXISTS watched";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();
            query = "DROP TABLE IF EXISTS casted_in";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();
            query = "DROP TABLE IF EXISTS movie ";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();
            query = "DROP TABLE IF EXISTS actor";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();
            query = "DROP TABLE IF EXISTS director";
            stmt = _connection.prepareStatement(query);
            stmt.executeUpdate();

        } catch(SQLException e){
            System.out.println(e.getMessage());
        } finally {
            try {
                if(_connection != null ) 
                    stmt.close();  
            } catch(SQLException e){
                System.out.println(e.getMessage());
            }              
        }
    }

    /* 
     - This main function is for helping you
     - You don't have to send your main function when you submit your code.
        
     - Use this template to continue coding. 
     - Just implement a function above, and call it with example parameters
     in main function below.
     */
    public static void main(String[] args) throws Exception {

        /* ***** DO NOT CHANGE THIS PART FOR YOUR OWN FAVOUR - BEGIN ***** */
		
        // Create new movie manager object
        MovieManager manager = new MovieManager();

        // Read databaseConfiguration.txt file for database settings
        manager.ReadDatabaseConfiguration();

        // Initialize connection with database and
        // Create connection object
        manager.InitializeConnection();

        /* ***** DO NOT CHANGE THIS PART FOR YOUR OWN FAVOUR - END ***** */

        // Rest of the functions can be called here for testing
       // manager.CreateTables();

        //Actor ac1 = new Actor(1,"Jean Reno");

        //manager.InsertActor(ac1);

        manager.DropTables();
        // manager.ParseFilesAndInsertData();                 
        /*List<Movie> movies = manager.ChangeRatingsOfMoviesLike("redemption");
        if (movies == null )
            System.out .println("THERE IS NO KIND OF THAT MOVIE");
        for(int i = 0; i< movies.size();i++){
            System.out.println(movies.get(i).getTitle());
        }
        
        
        System.out.println(movies.size());
        for (int i = 0; i < movies.size(); ++i)
        {
            Movie movie = movies.get(i);
            System.out.println("------");
            System.out.println("Movie: " + movie.getTitle() + " Id: " + movie.getMid());
            System.out.println("Year: " + movie.getYear() + " Rating: " + movie.getRating());
            Director director = movie.getDirector();
            System.out.println("Director: " + director.getName() + " Id: " + director.getDid());
        }
        
        manager.PrintViewStatsByGenre();
        manager.PrintViewStatsByMovie();
        manager.PrintAverageMovieRatingOfDirectors();
        
        manager.ChangeRatingsOfMoviesLike("SMaN", 7.9f);
        manager.DropTables();*/
        

    }
}
