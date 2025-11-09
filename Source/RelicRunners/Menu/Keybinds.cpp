// Fill out your copyright notice in the Description page of Project Settings.


#include "Keybinds.h"

UKeybinds::UKeybinds()
{
    KeyBinds.Add({ TEXT("Walk Forward"),       EKeys::W });
    KeyBinds.Add({ TEXT("Walk Left"),          EKeys::A });
    KeyBinds.Add({ TEXT("Walk Backward"),      EKeys::S });
    KeyBinds.Add({ TEXT("Walk Right"),         EKeys::D });
    KeyBinds.Add({ TEXT("Jump"),               EKeys::SpaceBar });
    KeyBinds.Add({ TEXT("Inventory"),          EKeys::I });
    KeyBinds.Add({ TEXT("Ability Selection"),  EKeys::U });
    KeyBinds.Add({ TEXT("Interact"),           EKeys::E });
    KeyBinds.Add({ TEXT("Map"),                EKeys::M });
    KeyBinds.Add({ TEXT("Pause"),              EKeys::Escape });
    KeyBinds.Add({ TEXT("Ping"),               EKeys::MiddleMouseButton });
    KeyBinds.Add({ TEXT("Basic Attack"),       EKeys::LeftMouseButton });
    KeyBinds.Add({ TEXT("Damage Ability"),     EKeys::Q });
    KeyBinds.Add({ TEXT("Utility Ability"),    EKeys::R });
    KeyBinds.Add({ TEXT("Defence Ability"),    EKeys::T });
    KeyBinds.Add({ TEXT("Ultimate Ability"),   EKeys::F });
    KeyBinds.Add({ TEXT("Health Potion"),      EKeys::H });
}