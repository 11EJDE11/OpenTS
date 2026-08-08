from pathlib import Path
import sys
import tempfile
import unittest

import yaml


TOOLS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOLS))

import extract
import extract_engine
import ini_inventory
import schema_validation
import section_selectors


OBJECT_TYPE_KEYS = (
    'Image',
    'AlphaImage',
    'CrushSound',
    'Crushable',
    'RadarInvisible',
    'Selectable',
    'LegalTarget',
    'Armor',
    'Strength',
    'Immune',
    'Insignificant',
    'HasRadialIndicator',
    'RadialColor',
    'IgnoresFirestorm',
    'Theater',
    'NewTheater',
    'Voxel',
)

GENERIC_OBJECT_TYPE_KEYS = OBJECT_TYPE_KEYS[:14]

RULES_OBJECT_TYPES = (
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
)


def loader_source(overrides=None, remove=None, append=None):
    overrides = overrides or {}
    remove = set(remove or ())
    lines = ['bool RulesClass::Objects(CCINIClass const & ini)', '{']
    for target in extract_engine.RULES_OBJECTS_LOADERS:
        if target in remove:
            continue
        receiver = overrides.get(
            target, 'ArtINI' if target == 'AnimTypes' else 'ini')
        expression = target if target == 'miss' else '%s[index]' % target
        lines.append('    %s->Read_INI(%s);' % (expression, receiver))
    lines.extend(append or ())
    lines.extend(['    return true;', '}'])
    return '\n'.join(lines)


def record(key, declared_in, member, receiver='ini', section='IniName', line=1):
    return {
        'key': key,
        'receiver': receiver,
        'suffix': 'Int',
        'section_expr': section,
        'default_expr': member,
        'local_lhs': False,
        'member': member,
        'value_type': 'integer',
        'declared_in': declared_in,
        'guard': None,
        'line': line,
        'src': 'synthetic.cpp',
    }


class RulesObjectsInventoryTests(unittest.TestCase):
    def test_current_inventory_and_object_contexts_are_exact(self):
        rows = extract_engine.discover_rules_objects_loaders()
        self.assertEqual(len(rows), 17)
        self.assertEqual(
            [(row['target'], row['receiver']) for row in rows],
            [
                ('HouseTypes', 'ini'),
                ('SuperWeaponTypes', 'ini'),
                ('AnimTypes', 'ArtINI'),
                ('BuildingTypes', 'ini'),
                ('AircraftTypes', 'ini'),
                ('UnitTypes', 'ini'),
                ('InfantryTypes', 'ini'),
                ('Weapons', 'ini'),
                ('BulletTypes', 'ini'),
                ('::Warheads', 'ini'),
                ('TerrainTypes', 'ini'),
                ('SmudgeTypes', 'ini'),
                ('OverlayTypes', 'ini'),
                ('ParticleTypes', 'ini'),
                ('ParticleSystemTypes', 'ini'),
                ('VoxelAnimTypes', 'ini'),
                ('miss', 'ini'),
            ],
        )

        contexts = extract_engine.object_type_loader_contexts()
        self.assertEqual(len(contexts), 12)
        self.assertEqual(
            {context['applies_to'] for context in contexts},
            set(RULES_OBJECT_TYPES) | {'AnimType'},
        )
        self.assertEqual(
            [context['receiver'] for context in contexts
             if context['applies_to'] == 'AnimType'],
            ['ArtINI'],
        )
        self.assertNotIn(
            'IsometricTileType',
            {context['applies_to'] for context in contexts},
        )

    def test_inventory_rejects_new_removed_duplicate_and_unclassified_calls(self):
        cases = {
            'unmapped loader MysteryTypes': loader_source(
                append=['    MysteryTypes[index]->Read_INI(ini);']),
            'expected loader BuildingTypes is no longer present': loader_source(
                remove=['BuildingTypes']),
            'duplicate loader BuildingTypes': loader_source(
                append=['    BuildingTypes[index]->Read_INI(ini);']),
            'unclassified INI receiver MysteryINI': loader_source(
                overrides={'BuildingTypes': 'MysteryINI'}),
        }
        tree = extract_engine.load_hierarchy()
        for expected, source in cases.items():
            with self.subTest(expected=expected):
                with tempfile.TemporaryDirectory() as folder:
                    Path(folder, 'rules.cpp').write_text(
                        source, encoding='latin-1')
                    with self.assertRaisesRegex(ValueError, expected):
                        extract_engine.discover_rules_objects_loaders(
                            folder, tree)


class CurrentObjectTypeContractTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.keys, _, _, _ = extract._extract_all(
            ini_inventory.load_manifest())

    def test_complete_object_type_reader_surface_is_present(self):
        records, _ = extract_engine.extract_file(
            Path(extract_engine.CODE_DIR, 'objtype.cpp'),
            'ObjectTypeClass',
            extract_engine.load_hierarchy(),
        )
        self.assertEqual(
            tuple(record['key'] for record in records),
            OBJECT_TYPE_KEYS,
        )
        for key in OBJECT_TYPE_KEYS:
            with self.subTest(key=key):
                self.assertIn(key, self.keys)
                all_types = {
                    applies_to
                    for scope in self.keys[key]['scopes']
                    for applies_to in scope['applies_to']
                    if applies_to.endswith('Type')
                }
                self.assertNotIn('IsometricTileType', all_types)

    def test_every_generic_read_has_exact_rules_and_anim_caller_scopes(self):
        tree = extract_engine.load_hierarchy()
        records, defaults = extract_engine.extract_file(
            Path(extract_engine.CODE_DIR, 'objtype.cpp'),
            'ObjectTypeClass',
            tree,
        )
        keys = extract_engine.build(
            {'ObjectTypeClass': records},
            {'ObjectTypeClass': defaults},
            tree,
            extract_engine.object_type_loader_contexts(tree=tree),
        )
        selector = section_selectors.identifier('object-type')
        for key in GENERIC_OBJECT_TYPE_KEYS:
            with self.subTest(key=key):
                scopes = [
                    scope for scope in keys[key]['scopes']
                    if extract_engine._scope_identity(scope).startswith(
                        'ObjectTypeClass::')
                ]
                self.assertEqual(len(scopes), 2)
                self.assertEqual(scopes[0]['applies_to'], list(RULES_OBJECT_TYPES))
                self.assertEqual(scopes[0]['file'], 'rules.ini')
                self.assertEqual(scopes[0]['section'], selector)
                self.assertEqual(scopes[1]['applies_to'], ['AnimType'])
                self.assertEqual(scopes[1]['file'], 'art.ini')
                self.assertEqual(scopes[1]['section'], selector)

    def test_image_has_exactly_four_caller_correct_scopes(self):
        scopes = self.keys['Image']['scopes']
        self.assertEqual(len(scopes), 4)
        self.assertEqual(scopes[0]['applies_to'], list(RULES_OBJECT_TYPES))
        self.assertEqual(scopes[0]['file'], 'rules.ini')
        self.assertEqual(
            scopes[0]['section'],
            section_selectors.identifier('object-type'),
        )
        self.assertEqual(scopes[1]['applies_to'], ['AnimType'])
        self.assertEqual(scopes[1]['file'], 'art.ini')
        self.assertEqual(
            scopes[1]['section'],
            section_selectors.identifier('object-type'),
        )
        self.assertEqual(scopes[2]['applies_to'], ['Tiberium'])
        self.assertEqual(scopes[2]['value_type'], 'integer')
        self.assertEqual(scopes[3]['applies_to'], ['BuildingType'])
        self.assertEqual(scopes[3]['file'], 'art.ini')
        self.assertEqual(scopes[3]['section'], section_selectors.image())
        self.assertTrue(all(scope['status'] == 'reviewed' for scope in scopes))

    def test_repeated_caller_contexts_reuse_compact_yaml_objects(self):
        image = self.keys['Image']['scopes']
        alpha_image = self.keys['AlphaImage']['scopes']
        self.assertIs(image[0]['applies_to'], alpha_image[0]['applies_to'])
        self.assertIs(image[1]['applies_to'], alpha_image[1]['applies_to'])
        self.assertIsNot(image[0]['section'], image[1]['section'])
        serialized = yaml.safe_dump({
            'Image': self.keys['Image'],
            'AlphaImage': self.keys['AlphaImage'],
        }, sort_keys=False)
        self.assertIn('&id', serialized)
        self.assertIn('*id', serialized)

    def test_anim_theater_chain_is_separate_and_ordered(self):
        scopes = self.keys['NewTheater']['scopes']
        normal = next(scope for scope in scopes
                      if scope['applies_to'] == list(RULES_OBJECT_TYPES))
        anim = next(scope for scope in scopes
                    if scope['applies_to'] == ['AnimType'])
        self.assertNotIn('read_from', normal)
        self.assertEqual(
            [(read['file'], read['section']) for read in anim['read_from']],
            [
                ('art.ini', section_selectors.image()),
                ('art.ini', section_selectors.identifier('object-type')),
            ],
        )

    def test_explicit_art_reader_remains_additive_for_all_loaded_types(self):
        voxel = next(
            scope for scope in self.keys['Voxel']['scopes']
            if scope['_provenance']['declared_in'] == 'ObjectTypeClass')
        self.assertEqual(
            voxel['applies_to'],
            sorted(set(RULES_OBJECT_TYPES) | {'AnimType'}),
        )
        self.assertEqual(voxel['file'], 'art.ini')
        self.assertEqual(voxel['section'], section_selectors.image())


class ScopeAdjudicationTests(unittest.TestCase):
    def setUp(self):
        self.tree = {
            'UnitTypeClass': 'ObjectTypeClass',
            'OverlayTypeClass': 'ObjectTypeClass',
            'AnimTypeClass': 'ObjectTypeClass',
        }
        self.contexts = [
            {'class': 'UnitTypeClass', 'applies_to': 'UnitType',
             'receiver': 'ini'},
            {'class': 'OverlayTypeClass', 'applies_to': 'OverlayType',
             'receiver': 'ini'},
        ]

    def test_unrelated_base_reader_keeps_one_descendant_scope_and_file(self):
        tree = {
            'AnimTypeClass': 'UnrelatedBaseClass',
            'UnitTypeClass': 'UnrelatedBaseClass',
        }
        keys = extract_engine.build({
            'UnrelatedBaseClass': [
                record('Shared', 'UnrelatedBaseClass', 'Value'),
            ],
        }, {}, tree, [])
        self.assertEqual(len(keys['Shared']['scopes']), 1)
        self.assertEqual(
            keys['Shared']['scopes'][0]['applies_to'],
            ['AnimType', 'UnitType'],
        )
        self.assertEqual(keys['Shared']['scopes'][0]['file'], 'rules.ini')

    def test_specialization_is_the_only_subset_subtraction(self):
        records = {
            'ObjectTypeClass': [
                record('Shared', 'ObjectTypeClass', 'BaseValue', line=1),
            ],
            'OverlayTypeClass': [
                record('Shared', 'OverlayTypeClass', 'OverlayValue', line=2),
            ],
        }
        keys = extract_engine.build(records, {}, self.tree, self.contexts)
        base = next(scope for scope in keys['Shared']['scopes']
                    if extract_engine._scope_identity(scope)
                    == 'ObjectTypeClass::BaseValue')
        self.assertEqual(base['applies_to'], ['OverlayType', 'UnitType'])

        stale = extract_engine.apply_adjudications(keys, {
            'Shared': {
                'ObjectTypeClass::BaseValue': {'verdict': 'distinct'},
                'OverlayTypeClass::OverlayValue': {
                    'verdict': 'distinct',
                    'specializes': 'ObjectTypeClass::BaseValue',
                },
            },
        })
        self.assertEqual(stale, [])
        self.assertEqual(base['applies_to'], ['UnitType'])
        self.assertEqual(base['shares_name_with'], 'OverlayValue')

    def test_different_file_or_selector_contexts_remain_additive(self):
        records = {
            'ObjectTypeClass': [
                record('Shared', 'ObjectTypeClass', 'BaseValue', line=1),
            ],
            'OverlayTypeClass': [
                record(
                    'Shared', 'OverlayTypeClass', 'OverlayValue',
                    receiver='ArtINI', section='GraphicName', line=2),
            ],
        }
        keys = extract_engine.build(records, {}, self.tree, self.contexts)
        base = next(scope for scope in keys['Shared']['scopes']
                    if extract_engine._scope_identity(scope)
                    == 'ObjectTypeClass::BaseValue')
        self.assertEqual(base['applies_to'], ['OverlayType', 'UnitType'])

        stale = extract_engine.apply_adjudications(keys, {
            'Shared': {
                'ObjectTypeClass::BaseValue': {'verdict': 'distinct'},
                'OverlayTypeClass::OverlayValue': {
                    'verdict': 'distinct',
                    'specializes': 'ObjectTypeClass::BaseValue',
                },
            },
        })
        self.assertTrue(any('same-file/same-section' in item for item in stale))
        self.assertEqual(base['applies_to'], ['OverlayType', 'UnitType'])

    def test_one_verdict_reviews_every_context_clone(self):
        contexts = self.contexts + [{
            'class': 'AnimTypeClass',
            'applies_to': 'AnimType',
            'receiver': 'ArtINI',
        }]
        records = {
            'ObjectTypeClass': [
                record('Shared', 'ObjectTypeClass', 'BaseValue', line=1),
            ],
            'TiberiumClass': [
                record('Shared', 'TiberiumClass', 'Variant', line=2),
            ],
        }
        keys = extract_engine.build(records, {}, self.tree, contexts)
        stale = extract_engine.apply_adjudications(keys, {
            'Shared': {
                'ObjectTypeClass::BaseValue': {
                    'verdict': 'distinct',
                    'note': 'shared parser note',
                },
                'TiberiumClass::Variant': {'verdict': 'distinct'},
            },
        })
        self.assertEqual(stale, [])
        clones = [
            scope for scope in keys['Shared']['scopes']
            if extract_engine._scope_identity(scope)
            == 'ObjectTypeClass::BaseValue'
        ]
        self.assertEqual(len(clones), 2)
        self.assertTrue(all(scope['status'] == 'reviewed' for scope in clones))
        self.assertTrue(all(scope['note'] == 'shared parser note'
                            for scope in clones))

    def test_adjudication_schema_accepts_stable_specialization_identity(self):
        errors = schema_validation.errors_for({
            'Strength': {
                'ObjectTypeClass::MaxStrength': {'verdict': 'distinct'},
                'OverlayTypeClass::DamagePoints': {
                    'verdict': 'distinct',
                    'specializes': 'ObjectTypeClass::MaxStrength',
                },
            },
        }, 'adjudications.schema.json', 'adjudications')
        self.assertEqual(errors, [])


if __name__ == '__main__':
    unittest.main()
