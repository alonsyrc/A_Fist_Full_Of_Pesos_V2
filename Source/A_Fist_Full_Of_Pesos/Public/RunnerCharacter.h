// Completa tu aviso de copyright en la p�gina de Descripci�n de la Configuraci�n del Proyecto.

#pragma once // Asegura que este archivo de encabezado solo se incluya una vez durante la compilaci�n.

#include "CoreMinimal.h" // Incluye los componentes esenciales del motor de Unreal.
#include "GameFramework/Character.h" // Incluye la clase base ACharacter.
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "RunnerCharacter.generated.h" // Genera el c�digo necesario para la clase ARunnerCharacter.

// Declaraci�n de clases y estructuras
class USpringArmComponent; // Declara la clase USpringArmComponent.
class UCameraComponent; // Declara la clase UCameraComponent.
class UInputMappingContext; // Declara la clase UInputMappingContext.
class UInputAction; // Declara la clase UInputAction.
struct FInputActionValue; // Declara la estructura FInputActionValue.

// Declaraci�n de una categor�a de log para la clase
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game) // Macro que indica que esta es una clase de Unreal Engine y especifica la configuraci�n del juego.
class A_FIST_FULL_OF_PESOS_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY() // Macro que genera el c�digo est�ndar necesario para las clases de Unreal Engine.
	UUserWidget* PauseWidget;
	UUserWidget* Widget;
	UUserWidget* GameOverWidget;

		// Propiedades visibles en cualquier lugar
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* SideViewCamera; // C�mara lateral.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SideViewCameraBoom;

	///** Contexto de mapeo de entrada */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext; // Contexto de mapeo de entrada predeterminado.

	///** Acci�n de entrada para saltar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* JumpAction; // Acci�n de entrada para saltar.

	///** Acci�n de entrada para moverse */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction; // Acci�n de entrada para moverse.

	///** Acci�n de entrada para mirar */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* LookAction; // Acci�n de entrada para mirar.

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* PauseAction; // Acci�n de entrada para mirar.

public:
	// Declaraci�n de la funci�n PlayJumpSound
	UFUNCTION()
		void PlayJumpSound(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
		USoundBase* RestartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
		USoundBase* CoinSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
		USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		UTextBlock* TextBlockDistancia;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		UTextBlock* TextBlockPesos;

	// Declaraci�n de la variable para el widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> BP_UI_Ref;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget2")
		UButton* ResumeButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget2")
		UButton* MainMenuButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget2")
		UButton* QuitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget2")
		TSubclassOf<UUserWidget> BP_UI_PauseWidget_Ref;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget3")
		UButton* GameOverRestartButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget3")
		UButton* GameOverMainMenuButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget3")
		UButton* GameOverQuitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget3")
		TSubclassOf<UUserWidget> BP_UI_GameOverWidget_Ref;
	// Contador de monedas
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
		int Coins;

	// M�todo para agregar monedas
	UFUNCTION(BlueprintCallable, Category = "Player")
		void AddCoins(int Amount);

	void PlaySound(USoundBase* sound);

	// Variable para almacenar la distancia recorrida
	float DistanceTraveled;

	// M�todo para actualizar la distancia recorrida
	void UpdateDistanceTraveled(float DeltaTime);

	// Llamada cada cuadro.
	virtual void Tick(float DeltaTime) override;

	// Llamada para vincular funcionalidades a la entrada del jugador.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void ResetMeGame();
	UFUNCTION()
	void ResumeGame();
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void QuitGame();
	UFUNCTION()
	void PauseGame(const FInputActionValue& Value);

	void AddPauseUI();

	void RemovePauseUI();

	void AddGameOverUI();

	void RemoveGameOverUI();

	// Obtiene el componente de la c�mara lateral.
	class UCameraComponent* GetSideViewCameraComponent() const
	{
		return SideViewCamera;
	}

	// Reinicia el nivel.
	void RestartLevel();

	void DelayedRestartLevel();

	void RestartLevelDelayed();

	// Llamada cuando ocurre una superposici�n.
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappepedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepREsult);
	
	// Establece los valores predeterminados para las propiedades de este personaje.
	ARunnerCharacter();



protected:
	/** Llamada para la entrada de movimiento */
	void Move(const FInputActionValue& Value);

	// Llamada cuando el juego comienza o cuando el personaje es generado.
	virtual void BeginPlay() override;

	void AddRuntimeUI();

	void RemoveRuntimeUI();

private:
	float zPosition; // Posici�n en el eje Z.
	FVector tempPosition = FVector(); // Posici�n temporal.
	FRotator tempRotation = FRotator();
	bool CanMove; // Indica si el personaje puede moverse.
	bool bIsPaused; // Indica si el personaje puede moverse.
};
