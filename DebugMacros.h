#pragma once

#define PRINT_SCREEN(Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT(Text))
#define PRINT_CONSOLE(Text) UE_LOG(LogTemp, Warning, TEXT(Text))

#define PRINT_SCREEN_F(Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define PRINT_CONSOLE_F(Format, ...) UE_LOG(LogTemp, Warning, TEXT(Format), ##__VA_ARGS__)