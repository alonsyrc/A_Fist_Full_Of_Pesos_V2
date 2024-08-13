#include "RunnerCharacter.h" // Incluye la definición de la clase ARunnerCharacter.
#include "RunnerCharacter.h"
#include "Components/CapsuleComponent.h" // Incluye la clase UCapsuleComponent.
#include "Camera/CameraComponent.h" // Incluye la clase UCameraComponent.
#include "GameFramework/SpringArmComponent.h" // Incluye la clase USpringArmComponent.
#include "GameFramework/CharacterMovementComponent.h" // Incluye la clase UCharacterMovementComponent.
#include "GameFramework/Controller.h" // Incluye la clase AController.
#include "EnhancedInputComponent.h" // Incluye la clase UEnhancedInputComponent.
#include "EnhancedInputSubsystems.h" // Incluye los subsistemas de entrada mejorada.
#include "InputActionValue.h" // Incluye la estructura FInputActionValue.
#include <WallSpike.h> // Incluye la clase AWallSpike.
#include <Spikes.h> // Incluye la clase ASpikes.
#include <Engine.h> // Incluye la clase UEngine.
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "CustomWidget.h"

ARunnerCharacter::ARunnerCharacter()// Establece valores predeterminados
{ // Configura este personaje para llamar a Tick() cada cuadro. Puedes desactivar esto para mejorar el rendimiento si no lo necesitas.
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);    // Inicializa el tamaño del cápsula de colisión.
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);  // Configura la respuesta de colisión.
    bUseControllerRotationPitch = false;  // Configura la rotación del controlador.
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    SideViewCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SideViewCameraBoom")); // Crea el Spring Arm para la cámara lateral.
    SideViewCameraBoom->SetupAttachment(RootComponent); // Adjunta el Spring Arm al Root Component del personaje
    SideViewCameraBoom->TargetArmLength = 500.0f; // La distancia de la cámara al personaje.
    SideViewCameraBoom->bUsePawnControlRotation = false; // La cámara no rota según el controlador del Pawn.
    SideViewCameraBoom->SetRelativeRotation(FRotator(-20.0f, -90.0f, 0.0f)); // Ajusta la rotación del Spring Arm.
    SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));    // Crea la cámara lateral.
    SideViewCamera->SetupAttachment(SideViewCameraBoom, USpringArmComponent::SocketName); // Adjunta la cámara al final del Spring Arm.
    SideViewCamera->bUsePawnControlRotation = false; // La cámara no rota según el controlador del Pawn.
    GetCharacterMovement()->bOrientRotationToMovement = false; // No orienta la rotación al movimiento.
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);   // Configura el movimiento del personaje.
    GetCharacterMovement()->GravityScale = 2.0f;
    GetCharacterMovement()->AirControl = 0.8f;
    GetCharacterMovement()->JumpZVelocity = 1050.0f;
    GetCharacterMovement()->GroundFriction = 3.0f;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->MaxFlySpeed = 650.0f;
    tempPosition = GetActorLocation();    // Configura la posición temporal y la posición en Z.
}

// Llamado cuando el juego comienza o cuando el personaje es generado
void ARunnerCharacter::BeginPlay()
{
    Super::BeginPlay();

    PauseWidget = CreateWidget<UUserWidget>(GetWorld(), BP_UI_PauseWidget_Ref);
    AddPauseUI();
    if (PauseWidget)
    {
        // Encuentra los botones en el widget
        ResumeButton = Cast<UButton>(PauseWidget->GetWidgetFromName("ResumeButton"));
        MainMenuButton = Cast<UButton>(PauseWidget->GetWidgetFromName("MainMenuButton"));
        QuitButton = Cast<UButton>(PauseWidget->GetWidgetFromName("QuitButton"));

        // Verifica si los botones fueron encontrados
        if (ResumeButton)
            ResumeButton->OnClicked.AddDynamic(this, &ARunnerCharacter::ResumeGame);
        if (MainMenuButton)
            MainMenuButton->OnClicked.AddDynamic(this, &ARunnerCharacter::ReturnToMainMenu);
        if (QuitButton)
            QuitButton->OnClicked.AddDynamic(this, &ARunnerCharacter::QuitGame);
    }

    Widget = CreateWidget<UUserWidget>(GetWorld(), BP_UI_Ref);
    if (Widget)
    {
        TextBlockDistancia = Cast<UTextBlock>(Widget->GetWidgetFromName("TextBlockDistancia"));
        TextBlockPesos = Cast<UTextBlock>(Widget->GetWidgetFromName("TextBlockPesos"));
        if (TextBlockDistancia)
            TextBlockDistancia->SetText(FText::FromString("Distancia"));// Establece el texto del TextBlock
        if (TextBlockPesos)
            TextBlockPesos->SetText(FText::FromString("0"));// Establece el texto del TextBlock
    }

    RemovePauseUI(); 
    AddRuntimeUI();
    // Añade un evento de superposición.
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::OnOverlapBegin);
    CanMove = true;

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))// Añadir contexto de mapeo de entrada.
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ARunnerCharacter::AddRuntimeUI()
{// Agrega el widget al viewport
    if (Widget)
        Widget->AddToViewport();    
}

void ARunnerCharacter::RemoveRuntimeUI()
{// Agrega el widget al viewport
    if (Widget)
        Widget->RemoveFromParent();
}

void ARunnerCharacter::AddPauseUI()
{// Muestra el widget de pausa
    if (PauseWidget)
        PauseWidget->AddToViewport();
}
void ARunnerCharacter::RemovePauseUI()
{ // Muestra el widget de pausa
    if (PauseWidget)
        PauseWidget->RemoveFromParent();
}

void ARunnerCharacter::Tick(float DeltaTime)
{// Llamado cada cuadro
    Super::Tick(DeltaTime);
    UpdateDistanceTraveled(DeltaTime);// Actualiza la distancia recorrida
    if (GetActorLocation().Z <= -20.0f)// Comprueba si la posición Z del personaje es -200
        PlaySound(RestartSound);  // Reproduce el sonido de reinicio      
    if (GetActorLocation().Z <= -200.0f)
        DelayedRestartLevel(); // Llama a la función RestartLevel
}

// Llamado para vincular funcionalidades a la entrada del jugador
void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))    // Vincula las acciones de entrada mejorada.
    {   
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);// Saltar
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARunnerCharacter::PlayJumpSound);// Saltar
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);// Saltar      
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunnerCharacter::Move);// Mover   
        EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Completed, this, &ARunnerCharacter::PauseGame);// Pausar
    }
}

void ARunnerCharacter::ReturnToMainMenu()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Main Menu Button")));
    UGameplayStatics::OpenLevel(this, FName("MainMenuMap")); // Carga el mapa MainMenuMap
}

void ARunnerCharacter::QuitGame()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Quit Button")));
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->ConsoleCommand("quit");    // Cierra el juego
    }
}

void ARunnerCharacter::ResumeGame()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("ResumeButton")));
    RemovePauseUI();
    AddRuntimeUI();
    bIsPaused = false;
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->SetPause(false);
        PlayerController->SetInputMode(FInputModeGameOnly());
        PlayerController->bShowMouseCursor = false;
    }
}

// Función para pausar el juego y mostrar el widget
void ARunnerCharacter::PauseGame(const FInputActionValue& Value)
{
    if (!bIsPaused)
    {
        AddPauseUI();
        RemoveRuntimeUI();
        bIsPaused = true;
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            PlayerController->SetPause(true);
            PlayerController->SetInputMode(FInputModeUIOnly());
            PlayerController->bShowMouseCursor = true;
        }
    }
    else
    {
        ResumeGame();
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Out Pausado: %s"), bIsPaused ? TEXT("true") : TEXT("false")));
}

// Función de movimiento
void ARunnerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>(); // Obtiene el vector de movimiento.
    if (Controller != nullptr)
    {
        if (CanMove)
        {    
            const FRotator Rotation = Controller->GetControlRotation(); // Obtiene la rotación del controlador.
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);  // Obtiene el vector hacia adelante basado en la rotación del controlador.     
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);   // Obtiene el vector hacia la derecha basado en la rotación del controlador.
            AddMovementInput(RightDirection, MovementVector.Y); // Añade el movimiento hacia adelante y hacia atrás.
            AddMovementInput(ForwardDirection, MovementVector.X); // Añade el movimiento lateral.
        }
    }
}

// Reinicia el nivel
void ARunnerCharacter::DelayedRestartLevel()
{   
    FTimerHandle UnusedHandle;// Establece un temporizador para reiniciar el nivel después de 2 segundos
    GetWorldTimerManager().SetTimer(UnusedHandle, this, &ARunnerCharacter::RestartLevelDelayed, 2.0f, false);
}

void ARunnerCharacter::RestartLevel()
{
    PlaySound(RestartSound);
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName())); // Reinicia el nivel actual.
}

// Función que reinicia el nivel después del delay
void ARunnerCharacter::RestartLevelDelayed()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName())); // Reinicia el nivel actual.
}

// Maneja las superposiciones
void ARunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != nullptr)
    {
        AWallSpike* WallSpike = Cast<AWallSpike>(OtherActor); // Intenta convertir OtherActor a AWallSpike.
        ASpikes* Spike = Cast<ASpikes>(OtherActor); // Intenta convertir OtherActor a ASpikes.
        if (WallSpike || Spike) // Si colisiona con un spike o una pared con espinas
        {
            GetMesh()->Deactivate(); // Desactiva el modelo del personaje.
            GetMesh()->SetVisibility(false); // Hace invisible el modelo del personaje.
            CanMove = false; // Deshabilita el movimiento.
            FTimerHandle UnusedHandle; // Establece un temporizador para reiniciar el nivel después de 2 segundos.
            GetWorldTimerManager().SetTimer(UnusedHandle, this, &ARunnerCharacter::RestartLevel, 2.0f, false);
        }
    }
}

void ARunnerCharacter::AddCoins(int Amount)
{
    Coins += Amount;
    if (TextBlockPesos)
        TextBlockPesos->SetText(FText::FromString(FString::Printf(TEXT("%d"), Coins))); // Establece el texto del TextBlock
    PlaySound(CoinSound);   // Reproduce el sonido de coin
}

void ARunnerCharacter::PlaySound(USoundBase* sound)
{
    if (sound != nullptr) // Verifica si el sonido no es nulo
    {
        UAudioComponent* AudioComponent = Cast<UAudioComponent>(GetComponentByClass(UAudioComponent::StaticClass()));
        if (AudioComponent)
        {
            if (!AudioComponent->IsPlaying()) // Verifica si el componente de audio no está reproduciendo un sonido
            {
                FString soundName = sound->GetName(); // Obtiene el nombre del sonido
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Sonido: %s"), *soundName));
                AudioComponent->SetSound(sound);
                AudioComponent->Play();
            }
        }
    }
}

void ARunnerCharacter::PlayJumpSound(const FInputActionValue& Value)
{   // Obtiene el componente de audio
    PlaySound(JumpSound);
}

void ARunnerCharacter::UpdateDistanceTraveled(float DeltaTime)
{
    FVector Velocity = GetVelocity();// Calcula la velocidad del personaje
    float DistanceThisFrame = Velocity.Size() * DeltaTime; // Calcula la distancia recorrida en este frame
    if (Velocity.Y > 0) // Movimiento hacia adelante
        DistanceTraveled += DistanceThisFrame;     // Actualiza la distancia total recorrida
    else if (Velocity.Y < 0) // Movimiento hacia atrás
        DistanceTraveled -= DistanceThisFrame; // Resta la distancia recorrida de la distancia total
    if (TextBlockDistancia) 
        TextBlockDistancia->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(DistanceTraveled / 100.0f)))); // Establece el texto del TextBlock  
}