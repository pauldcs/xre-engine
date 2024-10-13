import { Poppins } from 'next/font/google';
import type { ReactNode } from 'react';

const poppins = Poppins({
  weight: ['100', '200', '300', '400', '500', '600', '700', '800', '900'],
  subsets: ['latin'],
});

export default async function RootLayout({ children }: { children: ReactNode }) {
  return (
    <html className={poppins.className}>
      <body>{children}</body>
    </html>
  );
}
