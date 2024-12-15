import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "GyroCycle",
  description: "Self-balancing bicycle.",
  head: [
    [ 'link', { rel: 'icon', href: '/assets/images/logo-light.svg' } ]
  ],
  markdown: {
    math: true
  },
  cleanUrls: true,
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Getting Started', link: '/guide/'},
      { text: 'Glossary', link: '/glossary/' }
    ],

    logo: {
      light: '/assets/images/logo-light.svg',
      dark: '/assets/images/logo-dark.svg',
      alt: 'GyroCycle'
    },

    sidebar: [
      {
        text: 'Guide',
        items: [
          { text: 'Getting Started', link: '/guide/' },
          { text: 'A few Words of Caution', link: '/guide/words-of-caution' },
          { text: 'Prerequisites', link: '/guide/prerequisites' },
          { text: 'Bill of Materials (BoM)', link: '/guide/bom' },
          {
            text: 'Manufacturing',
            items: [
              { text: 'Soldering', link: '/guide/soldering' },
              { text: '3D-Printed Parts', link: '/guide/3d-parts' },
              { text: 'Laser-Cut Parts', link: '/guide/laser-cutting' },
              { text: 'Manufacturing the Flywheel Shaft', link: '/guide/manufacturing-shaft.md'}
            ]
          },
          { text: 'Assembly', link: '/guide/assembly' },
          { text: 'Software Setup', link: '/guide/software-setup' }
        ]
      },
      { text: 'Glossary', link: '/glossary/' }
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/epfl-cs358/2024fa-gyrocycle' }
    ]
  }
})
