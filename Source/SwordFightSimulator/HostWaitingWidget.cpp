// Fill out your copyright notice in the Description page of Project Settings.


#include "HostWaitingWidget.h"
#include <SocketSubsystem.h>

FString UHostWaitingWidget::GetServerIPAddress()
{
    FString IPAddress = TEXT("Unknown");

    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (SocketSubsystem)
    {
        bool bCanBindAll;
        IPAddress = SocketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll)->ToString(false);
    }
    return IPAddress;
}
