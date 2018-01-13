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

    public void setBucket(CengBucket bucket)
    {
        this.bucket = bucket;
    }

    public void setHashPrefix(String hashPrefix)
    {
        this.hashPrefix = hashPrefix;    
    }
	
	// Own Methods

	private String hashPrefix;
	private Boolean isVisited;
	private CengBucket bucket;

	public CengHashRow(String hashPrefix){
		this.hashPrefix = hashPrefix;
        if(hashPrefix == "0")
        {
		this.bucket = new CengBucket(0);
	    }
        else
        {
		this.bucket = new CengBucket(hashPrefix.length());
        }
    }
}
