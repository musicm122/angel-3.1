%include <factory.i>

%factory(Actor*, TextActor, FullScreenActor, Camera, PhysicsActor, ParticleActor);
%factory(DeveloperLog*, ConsoleLog, CompoundLog, FileLog, SystemLog);
%factory(MessageListener*, TextActor, FullScreenActor, Camera, PhysicsActor, ParticleActor, Actor);
%factory(Renderable*, TextActor, FullScreenActor, PhysicsActor, Camera, GridActor, ParticleActor, Actor);