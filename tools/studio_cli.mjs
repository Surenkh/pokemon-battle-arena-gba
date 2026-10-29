import path from "node:path"; import {pathToFileURL} from "node:url";
process.resourcesPath=path.resolve(process.env.GBA_STUDIO_RESOURCES);
await import(pathToFileURL(path.join(process.resourcesPath,"app/.vite/build/cli.js")));
