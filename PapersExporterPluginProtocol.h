//
//  PapersExporterPluginProtocol.h
//
//  Created by Alexander Griekspoor on Fri Jan 23 2007.
//  Copyright (c) 2007 Mekentosj.com. All rights reserved.
// 
//  For use outside of the Papers application structure, please contact
//  feedback@mekentosj.com
//  DO NOT REDISTRIBUTE WITHOUT ALL THE FILES THAT ARE CONTAINED IN THE PACKAGE THAT INCLUDED THIS FILE

#import <Cocoa/Cocoa.h>


/////////////////////////////////////////////////////////////
/*  Exporting records
Papers allows you through this protocol to write your own exporter plugins.
These can not only be used to export files but to any kind of data or to any kind of medium.
It's important to note that the plugin's method performExportOfRecords:toURL: runs in 
its own thread, although you normally would not have to worry about that, keep 
in mind that if you do fancy stuff you use "performMethodOnMainThread: to prevent
trouble. The plugin signals it's progress to the delegate using a number of delegate 
methods. Make sure you ALWAYS call the delegate methods to signal your progress.
A normal cycle of the plugin would be:

Delegate -> Plugin		- (BOOL) readyToPerformExport;						// Is the plugin Ready, if not importPreparationError is retrieved.

Delegate -> Plugin		- (void) performExportOfRecords: (NSDictionary *)records toURL: (NSURL *)url;	// Delegate provides plugin with records, initiates export.
	
		Delegate <- Plugin		- (void)didBeginExport:(id)sender;				// Plugin informs delegate that it has started.

		Delegate <- Plugin		- (void)updateStatus:(id)sender;				// Plugin informs delegate about status change (optional).

		Delegate <- Plugin		- (void)didEndExport:(id)sender;				// Plugin signals it's done.

Delegate -> Plugin		- (BOOL) successfulCompletion;						// Delegate asks plugin if success, if not exportCompletionError is retrieved.

Delegate -> Plugin		- (void) performCleanup;							// Allows the plugin to cleanup and reset for next export.

The structure of the dictionary containing objects to be exported is as follows. Note that not all fields maybe present except those marked as obligatory

Note that at the root of the dictionary contains one or more arrays named:
- papers, an array containing dictionaries representing a paper.
- authors, an array containing dictionaries representing an author.
- journals, an array containing dictionaries representing a journal.
- keywords, an array containing dictionaries representing a keyword.
- publicationTypes, an array containing dictionaries representing a publication type.

And if you return YES on - (BOOL) shouldOpenSavePanel; you also get:
- url, an NSURL object that was selected in the save panel

Each can only occur once and contains an array of the corresponding model dictionaries below.
In the method - (NSArray *) exportableTypes; you have to indicate what you would like to export.

You can using the uri to uniquely identify an object within papers. The uri looks something like
x-coredata://078EEF81-1736-4FA9-B794-18E70DD6FFA2/Paper/p599; 
By replacing the x-coredata scheme by papers:// you can create a link from anywhere on the system
back to this object, for instance by adding it to a custom exported HTML webpage.
NOTE: these uris are not cross system compatible and only work for a particular library. To create
true cross machine compatibility use the doi's or other identifiers. Papers also accepts urls
like papers://doi/10.123/jem23.23 Only when those are unavailable you can use the uri.

Below follows an example for each category
Unless otherwise noted NSStrings are expected for all field  

AUTHORS
- correspondence
- email
- firstName
- homepage
- initials
- lastName - required
- mugshot (NSImage)
- nickName
- notes
- uri

Example: 

<key>authors</key>
<array>
	<dict>
		<key>correspondence</key>
		<string>The European Bioinformatics Institute</string>
		<key>email</key>
		<string>mek@mekentosj.com</string>
		<key>firstName</key>
		<string>Alexander</string>
		<key>homepage</key>
		<string>http://mekentosj.com</string>
		<key>initials</key>
		<string>AC</string>
		<key>lastName</key>
		<string>Griekspoor</string>
		<key>mugshot</key>
		NSImage object
		<key>nickName</key>
		<string>Mek</string>
		<key>notes</key>
		<string>These are example notes</string>
		<key>uri</key>
		<string>A UNIQUE ID THAT CAN BE USED TO POINT TO THIS OBJECT WITHIN PAPERS</string>
	</dict>
	<dict>
		.. next author ..
	</dict>
</array>


JOURNALS
- abbreviation
- archives
- authorGuidelines
- cover (NSImage)
- currentissue
- etoc
- homepage
- impactFactor (NSNumber - Float)
- issn
- language
- name - required
- nlmID
- notes
- openAccess (NSNumber - BOOL)
- publisher
- startYear (NSNumber - Int)
- summary
- uri

Example: 

<key>journals</key>
<array>
	<dict>
		<key>abbreviation</key>
		<string>PLoS Biol.</string>
		<key>archives</key>
		<string>http://biology.plosjournals.org/perlserv/?request=get-archive&amp;issn=1545-7885</string>
		<key>authorGuidelines</key>
		<string>http://journals.plos.org/plosbiology/guidelines.php</string>
		<key>cover</key>
		NSImage object
		<key>currentissue</key>
		<string>Vol. 4(12) December 2006</string>
		<key>etoc</key>
		<string>http://biology.plosjournals.org/perlserv/?request=get-toc&amp;issn=1545-7885</string>
		<key>homepage</key>
		<string>http://biology.plosjournals.org/</string>
		<key>impactFactor</key>
		<real>14.2</real>
		<key>issn</key>
		<string>1545-7885</string>
		<key>language</key>
		<string>eng</string>
		<key>name</key>
		<string>PLoS Biology</string>
		<key>nlmID</key>
		<string>101183755</string>
		<key>notes</key>
		<string>More info here...</string>
		<key>openAccess</key>
		<true/>
		<key>publisher</key>
		<string>Public Library of Science</string>
		<key>startYear</key>
		<integer>2003</integer>
		<key>summary</key>
		<string>PLoS Biology is an open-access, peer-reviewed general biology journal published by the Public Library of Science (PLoS), a nonprofit organization of scientists and physicians committed to making the world's scientific and medical literature a public resource. New articles are published online weekly; issues are published monthly.</string>
		<key>uri</key>
		<string>A UNIQUE ID THAT CAN BE USED TO POINT TO THIS OBJECT WITHIN PAPERS</string>
	</dict>
	<dict>
		.. next journal ..
	</dict>
</array>


KEYWORDS
- identifier
- name - required
- qualifier
- type
- subtype
- uri

Example: 

<key>keywords</key>
<array>
	<dict>
		<key>identifier</key>
		<string>an identifier</string>
		<key>name</key>
		<string>Breast Cancer</string>
		<key>qualifier</key>
		<string>a qualifier</string>
		<key>subtype</key>
		<string>Major Topic</string>
		<key>type</key>
		<string>MeSH Heading</string>
		<key>uri</key>
		<string>A UNIQUE ID THAT CAN BE USED TO POINT TO THIS OBJECT WITHIN PAPERS</string>
	</dict>
	<dict>
	.. next keyword ..
	</dict>
</array>


PUBLICATION TYPES
- name - required
- uri

Example: 

<key>publicationTypes</key>
<array>
	<dict>
		<key>name</key>
		<string>Journal Article</string>
		<key>uri</key>
		<string>A UNIQUE ID THAT CAN BE USED TO POINT TO THIS OBJECT WITHIN PAPERS</string>
	</dict>
	<dict>
	.. next publication type ..
	</dict>
</array>


PAPERS
-abstract
-acceptedDate (NSDate)
-affiliation
-authors (NSArray of author dictionaries - see above)
-category
-doi
-image (NSImage)
-issue
-journal (NSArray with a single journal dictionaries - see above)
-keywords (NSArray of keywords dictionaries - see above)
-label
-language
-notes
-openAccess (NSNumber - BOOL)
-pages
-path (NOTE that providing a path to a pdf file will automatically invoke auto-archiving if enabled)
-pii
-pmid
-publicationTypes (NSArray of publication type dictionaries - see above)
-publishedDate (NSDate)
-receivedDate (NSDate)
-revisedDate (NSDate)
-status
-timesCited (NSNumber - Int)
-title
-uri
-url
-volume
-year (NSNumber - Int)

Example: 

<key>Papers</key>
<array>
	<dict>
		<key>abstract</key>
		<string>MicroRNAs (miRNAs) interact with target...</string>
		<key>acceptedDate</key>
		NSDate object
		<key>affiliation</key>
		<string>Computational Biology Center</string>
		<key>authors</key>
			... HERE AN ARRAY OF AUTHOR DICTIONARIES LIKE ABOVE ...
		<array/>
		<key>category</key>
		<string>Journal Article</string>
		<key>doi</key>
		<string>10.1371/journal.pbio.0020363</string>
		<key>image</key>
		NSImage object
		<key>issue</key>
		<string>3</string>
		<key>journal</key>
			... HERE AN ARRAY WITH A SINGLE JOURNAL DICTIONARY LIKE ABOVE ...
		<array/>
		<key>keywords</key>
			... HERE AN ARRAY OF KEYWORD DICTIONARIES LIKE ABOVE ...
		<array/>
		<key>label</key>
		<string>Cell Biology</string>
		<key>language</key>
		<string>eng</string>
		<key>notes</key>
		<string>Here more info...</string>
		<key>openAccess</key>
		<true/>
		<key>pages</key>
		<string>e363</string>
		<key>path</key>
		<string>/Users/griek/Documents/Papers/1550875.pdf</string>
		<key>pii</key>
		<string>0020363</string>
		<key>pmid</key>
		<string>15502875</string>
		<key>publicationTypes</key>
			... HERE AN ARRAY OF PUBLICATION TYPE DICTIONARIES LIKE ABOVE ...
		<array/>
		<key>publishedDate</key>
		NSDate object
		<key>receivedDate</key>
		NSDate object
		<key>revisedDate</key>
		NSDate object
		<key>status</key>
		<string>In press</string>
		<key>timesCited</key>
		<integer>12</integer>
		<key>title</key>
		<string>Human MicroRNA targets</string>
		<key>uri</key>
		<string>A UNIQUE ID THAT CAN BE USED TO POINT TO THIS OBJECT WITHIN PAPERS</string>
		<key>url</key>
		<string>http://biology.plosjournals.org/perlserv/?request=get-document&amp;doi=10.1371/journal.pbio.0020363</string>
		<key>volume</key>
		<string>2</string>
		<key>year</key>
		<integer>2006</integer>
	</dict>
</array>

SEE THE COMPLETE EXAMPLE PLIST THAT CAME WITH THIS PACKAGE. 

*/

/////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
/*  USING NSERRORS
NSErrors are returned by two of the methods below in order to allow
the main application to inform the user of problems that occur while 
trying to use the plugin.  
The domain should always be "Papers Exporter Error"

The user dictionary should have two keys:
"title" and "description".  By default, they will be used to create
 the text of an informative dialog as: "title:  description"
 So design your messages accordingly.  

 PLEASE LOCALIZE YOUR MESSAGES WITHIN YOUR PLUGIN
 Make sure the "title" and "description" keys are in an appropriate language by
 the time the application gets the NSError.  This will allow the bundle to be
 entirely self-contained.
 Even if you don't know any other languages, provide the structure within your
 plugin to allow others to do the localization.

///////////////////////////////////////////////////////////// 
	DEFINED ERROR CODES
    use these for creating NSErrors that help inform the user 
    why things aren't working (more may be added in response to
    developer feedback)
                               
0 - an unknown error                               
                               
// 1 series - problems with file/data
1 = Destination not writable
2 = No sufficient diskspace error
3 = Error parsing data
4 = Error writing file

// 100 series - networking problems
100 = No network available
101 = required host unreachable
102 = network timeout
103 = response format error
104 = URL opening error (NSWorkspace openURL: method returned NO)
                    
*/
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////


// THESE ARE METHODS THE PLUGIN DELEGATE IMPLEMENTS AND YOU SHOULD CALL

@protocol PapersExporterPluginDelegate <NSObject>

// Signal the delegate that you started the export process
// Allows the delegate to prepare the interface before displaying new data
- (void)didBeginExport:(id)sender;	

// Signal the delegate that you are done. The delegate will inform how things went so be sure to set any errors and be ready
// to answer didCompleteSuccessfully.
- (void)didEndExport:(id)sender;					

// Inform the delegate of a status change, use when statusString, exportedItems or itemsToExport changes.
// Status updates are automatically issued by calling any of the methods above
- (void)updateStatus:(id)sender;

@end




typedef enum MTExportLimit
{
    MTExportLimitLibrary = 0,
	MTExportLimitGroup = 1,
    MTExportLimitSelection = 2
} MTExportLimit;


// THESE ARE METHODS YOUR PLUGIN SHOULD IMPLEMENT
@protocol  PapersExporterPluginProtocol

// gives you a handle to the delegate object to which you deliver results and notify progress (see above)
// do not retain the delegate
- (id)delegate;
- (void)setDelegate: (id)del;

// this should return an array of all menu items as strings.  Menu titles should be as descriptive as possible,
// so that we don't have naming collisions.
- (NSArray *) menuTitles;

// if your plugin writes files return YES, Papers will run the save panel for you (with the
// accessoryView if required.
// 
// NOTE: that your plugin can import anything you like and doesn't have to start with a file. 
// You can open a sheet, interact with a website, fetch data etc. Just return NO here in that case.
- (BOOL) shouldShowSavePanel;

// indicate what filetypes your plugin can save, only relevant when you return YES to the above method.
// return only 1 to set as a requiredFileType to be more strict
- (NSArray *) allowedFileTypes;

// indicate what type of modelobjects you wish/can to export. Return an array with one or more of the following strings:
// @"papers", @"authors", @"journals", @"keywords", @"publicationTypes". NOTE that you usually want to get @"Papers" only 
// these do include the associated authors, journals, keywords and publication types. If only or in addition you wish to 
// receive a separate array of these objects add the corresponding key to the array return by exportableTypes.
- (NSArray *) exportableTypes;

// indicate the limit of objects you want to export. Return MTExportLimitLibrary to export the whole Papers library, MTExportLimitGroup
// to select the currently selected group only, or MTExportLimitSelection to only export the selected papers. Note that you could
// dynamically return this on the basis of a popup in your accessory view.
- (MTExportLimit) exportLimit;

// if you would like to show options while the save panel is shown, return a custom view here and it will
// be used as accessoryView. Otherwise return nil.
- (NSView *) accessoryView;

// if you would like to customize the savepanel even further, set one up and return your own. 
// Otherwise return nil for the default one.
- (NSSavePanel *) savePanel;

// indicate whether this plugins needs an internet connection, if so Papers will check first if there is one.
- (BOOL) requiresInternetConnection;

// the interface can be as complex as you wish, but please always show some form of progress and allow the
// user to cancel the export. If you do not wish to show anything, at least show a minimum UI to show 
// the progress (see example plugins).

// a method to make sure everything's set to go before starting, do some setup or tests here if necessary.
// and a method to find out what the problems are if things aren't set. See above for usage of errorCodes.
- (BOOL) readyToPerformExport;
- (NSError *) exportPreparationError;

// Return YES if you support cancelling the current import session (strongly advised).
- (BOOL)canCancelExport;

// this method is the main worker method and launches the export process, here you are handed over
// the dictionary containing the exported objects. It also contains a key "url" with the URL that was selected in the save panel, 
// this can be nil if you return NO to shouldShowOpenPanel, in case you want to ignore this argument anyway.
// NOTE that this method runs in a separate thread. Signal your progress to the delegate.
- (void) performExportOfRecords: (NSDictionary *)records;

// informs us that we should stop exporting. Since we're running in a thread we should check regularly
// if we have been cancelled
- (void) cancelExport;

// return the number of items you are about to export. As long as this value is nil an indeterminate
// progress bar is shown, the moment you return a non-nil value for both the progress will be shown to the user. 
// use in combination with the delegate method updateStatus: to push changes to the delegate and force an update.
- (NSNumber *)itemsToExport;
- (NSNumber *)exportedItems;

// return the current status of your plugin to inform the user. make sure these strings are localizable!
// use in combination with the delegate method updateStatus: to push changes to the delegate and force an update.
- (NSString *)statusString;

// A method to check whether the export finished properly
// and one to get at any errors that resulted. See above for usage of errorCodes.
- (BOOL) successfulCompletion;
- (NSError *) exportCompletionError;

// let the plugin get rid of any data that needs to be reset for a new export.
- (void) performCleanup;

@end

