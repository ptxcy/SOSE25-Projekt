import Docker, {ContainerCreateOptions} from 'dockerode';

const docker = new Docker({
    socketPath: "/var/run/docker.sock"
});
export const CONTAINER_PREFIX = "calculation_unit_"

export async function startCalculationUnit(instanceNumber: number, exposedPort: number): Promise<void> {
    const internalPort = exposedPort;

    const containerConfig: ContainerCreateOptions = {
        Image: '3d-calculation-unit',
        Env: [`SERVICE_PORT=${internalPort}`],
        ExposedPorts: {[`${internalPort}/tcp`]: {}},
        HostConfig: {
            PortBindings: {[`${internalPort}/tcp`]: [{HostPort: `${exposedPort}`}]},
            NetworkMode: 'sose25-projekt'
        },
        name: CONTAINER_PREFIX + instanceNumber,
    };

    const container = await docker.createContainer(containerConfig);
    await container.start();
    console.log(`Container gestartet: Intern auf Port ${internalPort}, Extern auf Port ${exposedPort}!`);
}

export async function startPongCalculationUnit(instanceNumber: number, exposedPort: number): Promise<void> {
    const internalPort = exposedPort;

    const containerConfig: ContainerCreateOptions = {
        Image: 'pong-calculation-unit',
        Env: [`SERVICE_PORT=${internalPort}`],
        ExposedPorts: {[`${internalPort}/tcp`]: {}},
        HostConfig: {
            PortBindings: {[`${internalPort}/tcp`]: [{HostPort: `${exposedPort}`}]},
            NetworkMode: 'sose25-projekt'
        },
        name: CONTAINER_PREFIX + instanceNumber,
    };

    const container = await docker.createContainer(containerConfig);
    await container.start();
    console.log(`Container gestartet: Intern auf Port ${internalPort}, Extern auf Port ${exposedPort}!`);
}

export async function stopContainer(containerName: string): Promise<void> {
    try {
        const container = docker.getContainer(containerName);
        await container.stop();
        //await container.kill()
        await container.remove({force: true});
        console.log(`Container ${containerName} wurde gestoppt.`);
    } catch (error) {
        console.error(`Fehler beim Stoppen des Containers ${containerName}:`, error);

    }
}
