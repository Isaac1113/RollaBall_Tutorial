// Fill out your copyright notice in the Description page of Project Settings.


#include "RollaBallPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARollaBallPlayer::ARollaBallPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create Components - Before this step they won't exist on our Actor, they've only been defined.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

	// Set the root component to be our mesh
	RootComponent = Mesh;
	// Attach the Spring Arm to the Mesh, Spring will now follow the Mesh transform
	SpringArm->SetupAttachment(Mesh);
	// Attach the camera to the spring arm, camera will now follow the spring arm transform
	Camera->SetupAttachment(SpringArm);

	// Set physics to true
	Mesh->SetSimulatePhysics(true);
	
	Mesh->OnComponentHit.AddDynamic(this, &ARollaBallPlayer::CustomOnHit);

}

// Called when the game starts or when spawned
void ARollaBallPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Account for mass in MoveForce and JumpImpulse
	MoveForce *= Mesh->GetMass();
	JumpImpulse *= Mesh->GetMass();
	
}

// Called to bind functionality to input
void ARollaBallPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Custom Input Axis Bindings
	InputComponent->BindAxis("MoveForward", this, &ARollaBallPlayer::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARollaBallPlayer::MoveRight);
	// Custom Action Bindings
	InputComponent->BindAction("Jump", IE_Pressed, this, &ARollaBallPlayer::Jump);
}

void ARollaBallPlayer::MoveRight(float Value)
{
	// Get the Right vector of the camera as it doesn't rotate and move the player in this direction based on the input and MoveForce
	const FVector Right = Camera->GetRightVector() * MoveForce * Value;
	Mesh->AddForce(Right);
}

void ARollaBallPlayer::MoveForward(float Value)
{
	const FVector Forward = Camera->GetForwardVector() * MoveForce * Value;
	Mesh->AddForce(Forward);
}

void ARollaBallPlayer::Jump()
{
	// Cap the number of jumps we can make
	if (JumpCount >= MaxJumpCount) { return; }

	// Apply an impulse to the mesh in the Z-Axis
	Mesh->AddImpulse(FVector(0, 0, JumpImpulse));

	// Track how many times we've jumped
	JumpCount++;
}

void ARollaBallPlayer::CustomOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get direction we hit the surfacec on the Z axis
	float HitDirection = Hit.Normal.Z;

	// GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, FString::Printf(TEXT("Z Normal: %f"), HitDirection));

	// If it's more than 0 then we hit something below us. 1 is flat, anything between is a slope.
	if (HitDirection > 0)
	{
		JumpCount = 0;
	}
}
