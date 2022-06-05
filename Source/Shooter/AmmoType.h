#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayNAme = "9mm"),
	EAT_AR UMETA(DisplayNAme = "Assault Rifle"),

	EAT_MAX UMETA(DisplayNAme = "DefaultMax")
};