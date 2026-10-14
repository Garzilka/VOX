#include "NotificationServiceImpl.h"

void NotificationServiceImpl::NotifyClients(const vector<string> clietnsUUID, const SBaseMessageData& Message)
{
	if (!cacheService)
	{
		CLIENT_ERROR("No cache servive for notify");
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
