import type { ExecuteCodePort } from '../exec.ports';

export function executeCodeCommand(ports: { execCodePort: ExecuteCodePort }) {
  return async (code: string) => {
    return ports.execCodePort({ code });
  };
}
