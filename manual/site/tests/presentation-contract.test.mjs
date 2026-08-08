import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';
import { load } from 'js-yaml';

const site = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const repository = resolve(site, '../..');
const readRepositoryFile = (path) => readFileSync(resolve(repository, path), 'utf8');

test('authored H3 headings use the shared subordinate scale and color', () => {
	const css = readRepositoryFile('manual/site/src/styles/custom.css');
	assert.match(css, /:root\s*{[^}]*--sl-text-h3:\s*1\.375rem;/s);
	assert.match(
		css,
		/@media\s*\(min-width:\s*50em\)\s*{\s*:root\s*{[^}]*--sl-text-h3:\s*1\.625rem;/s,
	);
	assert.match(
		css,
		/\.sl-markdown-content\s+\.sl-heading-wrapper\.level-h3\s*>\s*h3\s*{[^}]*color:\s*var\(--ots-ink\);/s,
	);
});

test('Image keeps four ordered public scopes and stable fragment identities', () => {
	const keys = load(readRepositoryFile('manual/data/ini-keys.yaml'));
	const scopes = keys.Image?.scopes;
	assert.ok(scopes, 'Image is missing from the generated INI catalog');
	assert.equal(scopes.length, 4);
	assert.deepEqual(
		scopes.map((scope) => ({
			file: scope.file,
			section: scope.section,
			value_type: scope.value_type,
			first_type: scope.applies_to[0],
		})),
		[
			{
				file: 'rules.ini',
				section: { kind: 'identifier', source: 'object-type' },
				value_type: 'string',
				first_type: 'AircraftType',
			},
			{
				file: 'art.ini',
				section: { kind: 'identifier', source: 'object-type' },
				value_type: 'string',
				first_type: 'AnimType',
			},
			{
				file: 'rules.ini',
				section: { kind: 'identifier', source: 'object-type' },
				value_type: 'integer',
				first_type: 'Tiberium',
			},
			{
				file: 'art.ini',
				section: { kind: 'image', fallback: 'object-type' },
				value_type: 'string',
				first_type: 'BuildingType',
			},
		],
	);

	const ordinary = [
		'AircraftType',
		'BuildingType',
		'BulletType',
		'InfantryType',
		'OverlayType',
		'ParticleSystemType',
		'ParticleType',
		'SmudgeType',
		'TerrainType',
		'UnitType',
		'VoxelAnimType',
	];
	assert.deepEqual([...scopes[0].applies_to].sort(), ordinary.sort());
	assert.deepEqual(scopes[1].applies_to, ['AnimType']);
	assert.deepEqual(scopes[2].applies_to, ['Tiberium']);
	assert.deepEqual(scopes[3].applies_to, ['BuildingType']);
	assert.ok(scopes.every((scope) => !scope.applies_to.includes('IsometricTileType')));
});
