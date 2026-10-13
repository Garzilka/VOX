#include "NotificationServiceImpl.h"

void NotificationServiceImpl::NotifyClients(const vector<string> clietnsUUID, const SBaseMessageData Message)
{
	if (!cacheService)
	{
		ERROR_CLIENT("No cache servive fot notify");
		return;
	}
	for (const string& clientUUID : clietnsUUID)
	{
		auto sharedClient = cacheService->findAuthClient(clientUUID);
		if (!sharedClient)
		{
			sharedClient = cacheService->findUnauthClient(clientUUID);
		}
		if (!sharedClient) continue;

		sharedClient->sendMessage(Message);
	}
}
