'use server';

import { execCodeAdapter } from '../infrastructure/exec';
import { executeCodeCommand } from './commands/run-code.command';

export type ExecuteCodeServerAction = (code: string) => Promise<string>;
export const execCode: ExecuteCodeServerAction = executeCodeCommand({
  execCodePort: execCodeAdapter,
});
