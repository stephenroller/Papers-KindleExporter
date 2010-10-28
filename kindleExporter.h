#import <Cocoa/Cocoa.h>
#import <CoreServices/CoreServices.h>
#import "JSON/Classes/JSON.h"
#import "PapersExporterPluginProtocol.h"

#define APPEND_KINDLE_PATH @"/documents/Papers/"
#define COLLECTION_NAME @"Papers2@en-US"
#define KINDLE_LOCAL_PREFIX @"/mnt/us"


@interface KindleExporter : NSObject <PapersExporterPluginProtocol>
{
	id delegate;
	
	NSNumber *itemsToExport;
	NSNumber *exportedItems;
	NSString *statusString;	
	
    NSError *exportError;
}

- (id)delegate;
- (void)setDelegate:(id)newDelegate;

- (NSNumber *)itemsToExport;
- (void)setItemsToExport:(NSNumber *)newItemsToExport;

- (NSNumber *)exportedItems;
- (void)setExportedItems:(NSNumber *)newExportedItems;
- (void)incrementExportedItemsWith: (int)value;

- (NSString *)statusString;
- (void)setStatusString:(NSString *)newStatusString;

- (NSError *)exportError;
- (void)setExportError:(NSError *)newExportError;

- (BOOL)copyToFolder:(NSString *)srcFile dest:(NSString *)destFolder overwrite:(BOOL)overwrite error:(NSError*)err;


@end
