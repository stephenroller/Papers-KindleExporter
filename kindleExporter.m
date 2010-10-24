#import "kindleExporter.h"

@interface KindleExporter (private)

BOOL shouldContinueExport;

@end


@implementation KindleExporter

#pragma mark - 
#pragma mark Init

- (id) init {
    self = [super init];
	if ( self != nil ) {	
		// space for early setup
	}
	return self;    
}

- (void) awakeFromNib {
	// setup nib if necessary
}

- (void) dealloc {
    // cleanup last items here
	[super dealloc];
}


#pragma mark -
#pragma mark Accessors

- (id)delegate
{
	return delegate;
}

- (void)setDelegate:(id)newDelegate
{
	delegate = newDelegate;
}


- (NSNumber *)itemsToExport
{
	return itemsToExport;
}

- (void)setItemsToExport:(NSNumber *)newItemsToExport
{
	[newItemsToExport retain];
	[itemsToExport release];
	itemsToExport = newItemsToExport;
	
	// inform our delegate to update the status
	id <PapersExporterPluginDelegate> del = [self delegate];
	[del updateStatus: self];
}


- (NSNumber *)exportedItems
{
	return exportedItems;
}

- (void)setExportedItems:(NSNumber *)newExportedItems
{
	[newExportedItems retain];
	[exportedItems release];
	exportedItems = newExportedItems;
	
	// inform our delegate to update the status
	id <PapersExporterPluginDelegate> del = [self delegate];
	[del updateStatus: self];
}

- (void)incrementExportedItemsWith: (int)value{
	int old = [[self exportedItems]intValue];
	[self setExportedItems: [NSNumber numberWithInt: old+value]];
}


- (NSString *)statusString
{
	return statusString;
}

- (void)setStatusString:(NSString *)newStatusString
{
	[newStatusString retain];
	[statusString release];
	statusString = newStatusString;
	
	// inform our delegate to update the status
	id <PapersExporterPluginDelegate> del = [self delegate];
	[del updateStatus: self];
}


- (NSError *)exportError
{
	return exportError;
}

- (void)setExportError:(NSError *)newExportError
{
	[newExportError retain];
	[exportError release];
	exportError = newExportError;
}



#pragma mark -
#pragma mark Interface interaction methods

- (NSArray *) menuTitles{
	return [NSArray arrayWithObjects: NSLocalizedStringFromTableInBundle(@"Kindle", nil, [NSBundle bundleForClass: [self class]], @"Kindle title"), nil];
}


#pragma mark -
#pragma mark Open Panel methods

- (BOOL) shouldShowSavePanel{
	return NO;
}

- (NSArray *) allowedFileTypes{
	return nil;
}

- (NSArray *) exportableTypes{
	return [NSArray arrayWithObjects: @"papers", nil];
}

- (MTExportLimit) exportLimit{
	return MTExportLimitSelection;
}


- (BOOL) requiresInternetConnection{
	return NO;
}

- (BOOL) canCancelExport{
	return YES;
}

- (NSSavePanel *) savePanel{
	return nil;
}

- (NSView *) accessoryView{
	return nil;
}



#pragma mark -
#pragma mark Preparation methods

// a method to make sure everything's set to go before starting, do some setup or tests here if necessary.
// and a method to find out what the problems are if things aren't set. See above for usage of errorCodes.
- (BOOL) readyToPerformExport{
	
	// do some setup here if necessary
	shouldContinueExport = YES;
	[self setExportedItems: [NSNumber numberWithInt: 0]];
	return YES;
}

- (NSError *) exportPreparationError{
	return exportError;
}


#pragma mark -
#pragma mark Export methods

NSString* getKindlePath()
{
	NSString *kindlePath = nil;
	
	NSArray *resourceKeys = [NSArray arrayWithObjects:
							 NSURLLocalizedNameKey, NSURLEffectiveIconKey, nil];
	NSArray *volumeURLs = [[NSFileManager defaultManager]
						   mountedVolumeURLsIncludingResourceValuesForKeys:resourceKeys
						   options:0];
	NSURL *curvol;
	NSEnumerator *iterator = [volumeURLs objectEnumerator];
	
	while (curvol = [iterator nextObject])
	{
		NSString *displayName = [[NSFileManager defaultManager] displayNameAtPath:[curvol path]];
		if ([displayName isEqual:@"Kindle"])
		{
			kindlePath = [curvol path];
			break;
		}
	}
	
	if (kindlePath)
	{
		NSLog(@"Found the Kindle: %@\n", kindlePath);
	}
	
	return kindlePath;
}

- (NSString*)sha1hash:(NSString *)input {
	NSTask *SHA1Task = [[[NSTask alloc] init] autorelease];
	
	[SHA1Task setLaunchPath:@"/usr/bin/openssl"];
	[SHA1Task setArguments:[NSArray arrayWithObject:@"sha1"]];
	
	NSPipe *SHA1StdinPipe = [NSPipe pipe];
	[SHA1Task setStandardInput:SHA1StdinPipe];
	[[SHA1StdinPipe fileHandleForWriting] writeData:[input dataUsingEncoding:NSASCIIStringEncoding]];
	[[SHA1StdinPipe fileHandleForWriting] closeFile];
	
	NSPipe *SHA1StdoutPipe = [NSPipe pipe];
	[SHA1Task setStandardOutput:SHA1StdoutPipe];
	
	[SHA1Task launch];
	
	NSData *SHA1Data = [[SHA1StdoutPipe fileHandleForReading] readDataToEndOfFile];
	[SHA1Task waitUntilExit];
	
	NSString *hash = [[NSString alloc] initWithData:SHA1Data encoding:NSASCIIStringEncoding];
	hash = [hash substringToIndex:40];
	
	return hash;
	
}

- (BOOL)copyToFolder:(NSString *)srcFile dest:(NSString *)destFolder overwrite:(BOOL)overwrite error:(NSError*)err {
	NSFileManager *fileman = [NSFileManager defaultManager];
	
	NSString *fileName = [srcFile lastPathComponent];
	NSString *destFile = [destFolder stringByAppendingPathComponent:fileName];
	
	NSLog(@"Copying '%@' to '%@'...\n", srcFile, destFile);
	
	if (![fileman fileExistsAtPath:destFolder])
	{
		[fileman createDirectoryAtPath:destFolder 
		   withIntermediateDirectories:YES 
							attributes:nil 
								 error:&err];
	}
	
	if (overwrite && [fileman fileExistsAtPath:destFile])
	{
		// delete the file!
		[fileman removeItemAtPath:destFile error:&err];
	}
	
	if (!err && [fileman copyItemAtPath:srcFile toPath:destFile error:&err])
	{
		NSLog(@"Copy successful.\n");
		return YES;
	}
	else
	{
		NSLog(@"ERROR: %@\n", [err description]);
		return NO;
	}
}

- (void) performExportOfRecords: (NSDictionary *)records{
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
	
	NSArray *paperArray = [records objectForKey: @"papers"];
	[self setItemsToExport: [NSNumber numberWithInt: [paperArray count]]];
	
	id <PapersExporterPluginDelegate> del = [self delegate];
	[del didBeginExport: self];

	NSError *err = nil;
	
	NSString *kindlePath = getKindlePath();
	NSString *exportDir = [kindlePath stringByAppendingPathComponent:APPEND_KINDLE_PATH];
	
	if (kindlePath == nil)
	{
		NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
        [userInfo setObject:@"Could not locate your Kindle!" forKey:NSLocalizedDescriptionKey];
		[userInfo setObject:@"Please make sure your Kindle is connected." forKey:NSLocalizedRecoverySuggestionErrorKey];
		
        
		err = [NSError errorWithDomain:@"KindleExportController" code:99 userInfo: userInfo];
		[self setExportError:err];
		
		[del didEndExport:self];
		[pool release];
		
		return;
	}
	
	// we also have to update the collections json
	//NSString *jsonFile = [kindlePath stringByAppendingPathComponent:@"system/collections.json"];
	//NSString *jsonString = [NSString stringWithContentsOfFile:jsonFile encoding:NSASCIIStringEncoding error:&err];
	
	//NSDictionary *jsonDict = [jsonString JSONValue];
	// FIXME: if COLLECTION_NAME doesn't exist, we should create it and items.
	//NSMutableArray *items = [[jsonDict objectForKey:COLLECTION_NAME] objectForKey:@"items"];
	
	
	// Iterate over each paper
	id paper;
	NSEnumerator *papers = [paperArray objectEnumerator];
	while(paper = [papers nextObject]) {
		NSString *filePath = [paper valueForKey:@"path"];
		if (!filePath || [filePath isEqualToString:@"(null)"])
		{
			[self incrementExportedItemsWith: 1];
			continue;
		}
		
		
		NSLog(@"Copying '%@' to kindle...", filePath);
		
		NSString *kindleFilename = [KINDLE_LOCAL_PREFIX stringByAppendingPathComponent:APPEND_KINDLE_PATH];
		kindleFilename = [kindleFilename stringByAppendingPathComponent:[filePath lastPathComponent]];
		NSLog(@"kindle filename: %@", kindleFilename);
		//NSString *hash = [@"*" stringByAppendingString:[self sha1hash:kindleFilename]];
		//[items removeObject:hash];
		//[items addObject:hash];
		
		
		// should we continue?
		if (!shouldContinueExport) {
			break;
		}
		
		if (![self copyToFolder:filePath dest:exportDir overwrite:YES error:err])
		{
			break;
		}
				
		// update count (informs delegate already through updateStatus)
		[self incrementExportedItemsWith: 1];
		
	}
	
	//[[jsonDict objectForKey:COLLECTION_NAME] setObject:items forKey:@"items"];
	
	//[[jsonDict JSONRepresentation] writeToFile:jsonFile 
	//								atomically:YES 
	//								  encoding:NSASCIIStringEncoding
	//									 error:&err];
	
	
	if(err) {
		NSLog(@"Error: ", [err description]);
		NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
        [userInfo setObject: @"Error while copying to Kindle" forKey:NSLocalizedDescriptionKey];
        [self setExportError: [NSError errorWithDomain: @"KindleExportController" code: 1 userInfo: userInfo]];
	}
	
	
	
	
	// cleanup
	
	// done, let the delegate know
	[del didEndExport: self];

	// cleanup nicely
	[pool release];
}


- (void) cancelExport{
	shouldContinueExport = NO;
	[self setStatusString: NSLocalizedStringFromTableInBundle(@"Cancelling export...", nil, [NSBundle bundleForClass: [self class]], @"Cancelling export...")];
}



#pragma mark -
#pragma mark Cleanup methods

// A method to check whether the export finished properly
// and one to get at any errors that resulted. See above for usage of errorCodes.
- (BOOL) successfulCompletion{
	// we simply check whether we caught an error
	return (exportError == nil);
}

- (NSError *) exportCompletionError{
	return exportError;
}

	// let the plugin get rid of any data that needs to be reset for a new export.
- (void) performCleanup{
	[itemsToExport release];
	itemsToExport = nil;
	
	[exportedItems release];
	exportedItems = nil;
	
	[statusString release];
	statusString = nil;
	
	[exportError release];
	exportError = nil;
	
}


@end
