import java.util.ArrayList;

public class CengBucketList {

	public void addCoin(CengCoin coin)
	{
		// TODO: Empty Implementation
	}
	
	public void searchCoin(Integer key)
	{
		// TODO: Empty Implementation
		
	}
	
	public void print()
	{
		// TODO: Empty Implementation
	}
	
	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public int bucketCount()
	{
		// TODO: Return all bucket count.
		return this.bucketList.size();
	}
	
	public CengBucket bucketAtIndex(int index)
	{
		// TODO: Return corresponding bucket at index.
		return this.bucketList.get(index);
	}
	
	// Own Methods
	public ArrayList<CengBucket> bucketList;

	public CengBucketList()
	{
		this.bucketList = new ArrayList<CengBucket>();
	}
}
 