public class CengHashRow {

	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
	public String hashPrefix()
	{
		// TODO: Return row's hash prefix (such as 0, 01, 010, ...)
		return this.hashPrefix;		
	}
	
	public CengBucket getBucket()
	{
		// TODO: Return the bucket that the row points at.
		return this.bucket;		
	}
	
	public boolean isVisited()
	{
		// TODO: Return whether the row is used while searching.
		return this.isVisited;		
	}
	
	// Own Methods

	String hashPrefix;
	Bool isVisited;
	CengBucket bucket;

	public CengHashRow(String hashPrefix){
		this.hashPrefix = hashPrefix;
		this.bucket = new CengBucket(hashPrefix.length());
	}
}
