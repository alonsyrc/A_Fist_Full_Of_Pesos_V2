// Fill out your copyright notice in the Description page of Project Settings.
// Completa tu aviso de copyright en la p�gina de Descripci�n de la Configuraci�n del Proyecto.

#include "WallSpike.h" // Incluye el archivo de encabezado de la clase AWallSpike

// Constructor de la clase AWallSpike
AWallSpike::AWallSpike()
{
    // Habilita la capacidad de que este actor se actualice cada cuadro
    PrimaryActorTick.bCanEverTick = true;
}

// Funci�n llamada cuando el juego comienza o cuando se genera el objeto
void AWallSpike::BeginPlay()
{
    // Llama a la implementaci�n de la clase base de BeginPlay
    Super::BeginPlay();

    // Establece la velocidad del componente ra�z a un vector (0, 25, 0)
    // Esto har� que el objeto se mueva en el eje Y
    this->GetRootComponent()->ComponentVelocity = FVector(0, 25, 0);
}

// Funci�n llamada cada cuadro para actualizar el actor
void AWallSpike::Tick(float DeltaTime)
{
    // Llama a la implementaci�n de la clase base de Tick
    Super::Tick(DeltaTime);

    // Actualiza la ubicaci�n del actor sumando un desplazamiento en el eje Y
    // La magnitud del desplazamiento es 350 unidades por segundo, escalada por DeltaTime
    SetActorLocation(GetActorLocation() + FVector(0, 350 * DeltaTime, 0), true);
}
