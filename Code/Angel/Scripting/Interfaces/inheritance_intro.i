%include <factory.i>

%factory(Actor*, TextActor, FullScreenActor, MazeFinder, Camera, PhysicsActor, ParticleActor);
%factory(DeveloperLog*, ConsoleLog, CompoundLog, FileLog, SystemLog);
%factory(MessageListener*, TextActor, MazeFinder, Camera, FullScreenActor, PhysicsActor, ParticleActor, Actor);
%factory(Renderable*, TextActor, MazeFinder, FullScreenActor, Camera, PhysicsActor, Actor, GridActor, ParticleActor);