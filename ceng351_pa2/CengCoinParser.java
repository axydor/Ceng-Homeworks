import java.io.IOException;
import java.util.ArrayList;
import java.io.*;

public class CengCoinParser {

	public static ArrayList<CengCoin> parseCoinsFromFile(String filename)
	{
		ArrayList<CengCoin> coinList = new ArrayList<CengCoin>();
				
		// You need to parse the input file in order to use GUI tables.
		// TODO: Parse the input file, and convert them into CengCoins
		BufferedReader br=null;
		try 
		{		
		br = new BufferedReader(new FileReader(filename));
		String line;
		while( (line = br.readLine()) != null )
		{
			String[] arr = line.split("\\|");
			int key = Integer.parseInt(arr[1]);
			String name = arr[2];
			String value = arr[3];
			String currency = arr[4];
            CengCoin coin = new CengCoin(key,name,value,currency);
            coinList.add(coin);
		}

		}
		catch(Exception ex)
		{
			System.out.println(ex.getMessage());
		}
        finally
        {
            try {
                if (br != null) 
                {
                    br.close();
                }
            } 
            catch (IOException ex) 
            {
                System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
                //throw ex;
            }
        }
		return coinList;
	}
	
	public static void startParsingCommandLine() throws IOException
	{
		// TODO: Start listening and parsing command line -System.in-.
		// There are 4 commands:
		// 1) quit : End the app, gracefully. Print nothing, call nothing.
		// 2) add : Parse and create the coin, and call CengCoinExchange.addCoin(newlyCreatedCoin).
		// 3) search : Parse the key, and call CengCoinExchange.searchCoin(parsedKey).
		// 4) print : Print the whole hash table with the corresponding buckets, call CengCoinExchange.printEverything().

		// Commands (quit, add, search, print) are case-insensitive.
		BufferedReader br=null;
		try 
		{
			br = new BufferedReader(new InputStreamReader(System.in));
			String line = "";
			
            while( (line  = br.readLine()) != null) {
                
                String[] arr = line.split("\\|");
				
                if(arr[0].equalsIgnoreCase("add"))
				{
					int key = Integer.valueOf(arr[1]);
					String name = arr[2];
					String value = arr[3];
					String currency = arr[4];
					CengCoin coin = new CengCoin(key,name,value,currency);
					CengCoinExchange.addCoin(coin);
				}

				else if (arr[0].equalsIgnoreCase("search"))
				{
					int key = Integer.valueOf(arr[1]);
					CengCoinExchange.searchCoin(key);				
				}

				else if (arr[0].equalsIgnoreCase("print"))
				{
					CengCoinExchange.printEverything();
				}

                else if (arr[0].equalsIgnoreCase("quit" ))
                {
                    break;
                }

                else
                {
                    System.out.println("Error");
                }
			}

		}
		catch (Exception ex)
		{
            System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
            throw ex;
        } 
        finally
        {
            try {
                if (br != null) 
                {
                    br.close();
                }
            } 
            catch (IOException ex) 
            {
                System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
                throw ex;
            }
        }
	}
}
